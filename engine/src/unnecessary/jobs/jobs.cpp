#include <unnecessary/application.h>
#include <unnecessary/jobs/jobs.h>

#include <utility>

#ifdef WIN32

#include <windows.h>

#endif
namespace un {

    void LambdaJob::operator()(un::JobWorker *worker) {
        callback();
    }

    LambdaJob::LambdaJob(LambdaJob::Callback callback) : callback(std::move(callback)) {}

    u32 JobSystem::enqueue(Job *job) {
        return enqueue(job, true);
    }

    u32 JobSystem::enqueue(Job *job, bool alsoMarkForExecution) {
        if (job == nullptr) {
            throw std::runtime_error("Unable to enqueue a null job");
        }
        u32 id;
        {
            std::lock_guard<std::mutex> guard(graphUsage);
            id = tasks.push(job);
        }
        if (alsoMarkForExecution) {
            markForExecution(id);
        }
        return id;
    }

    u32 JobSystem::enqueue(u32 dependsOn, Job *job) {
        u32 id = enqueue(job, false);
        addDependency(id, dependsOn);
        return id;
    }

    void JobSystem::addDependency(u32 from, u32 to) {
        {
            std::lock_guard<std::mutex> guard(graphUsage);
            tasks.connect(to, from, un::JobDependencyType::eRequired);
            tasks.connect(from, to, un::JobDependencyType::eRequirement);
        }
    }

    u32 JobSystem::enqueue(u32 dependsOn, un::LambdaJob::Callback callback) {
        return enqueue(dependsOn, new LambdaJob(std::move(callback)));
    }

#define N_JOBS_WARNING 32

    bool JobSystem::nextJob(Job **result, u32 *id) {
        u32 next;
        {
            std::lock_guard<std::mutex> guard(queueUsage);

            if (awaitingExecution.empty()) {
                return false;
            }
            next = awaitingExecution.front();
            *id = next;
            awaitingExecution.pop();
        }
        if (awaitingExecution.size() > N_JOBS_WARNING) {
            LOG(WARN) << "There are " << YELLOW(awaitingExecution.size()) << " jobs awaiting execution.";
        }
        *result = getJob(next);
        return true;
    }

    void JobSystem::awakeSomeoneUp() {
        for (JobWorker &worker : workers) {
            if (!worker.isAwake()) {
                // Wake up worker
                worker.awake();
                break;
            }
        }
    }

    JobSystem::JobSystem(un::Application &application) : queueUsage(), graphUsage() {
        size_t nCores = std::thread::hardware_concurrency();
        if (nCores == 0) {
            nCores = 4;
        }
        LOG(INFO) << "Using " << GREEN(nCores) << " workers for job system.";
        workers.reserve(nCores);
        for (size_t i = 0; i < nCores; ++i) {
            workers.emplace_back(application, this, i);
        }
    }

    JobSystem::~JobSystem() = default;

    u32 JobSystem::enqueue(un::LambdaJob::Callback callback) {
        return enqueue(new LambdaJob(std::move(callback)));
    }

    Job *JobSystem::getJob(u32 id) {
        Job *job;
        if (tasks.try_get_vertex(id, job)) {
            return job;
        }
        return nullptr;
    }

    JobWorker::JobWorker(
            JobWorker &&copy
    ) noexcept: thread(copy.thread),
                jobSystem(copy.jobSystem),
                running(copy.running),
                waiting() {
    }

    JobWorker::JobWorker(
            un::Application &application,
            JobSystem *jobSystem,
            size_t index
    ) : thread(new std::thread(&JobWorker::workerThread, this)),
        jobSystem(jobSystem),
        waiting(),
        running(true),
        index(index) {
        RenderingDevice &renderingDevice = application.getRenderer().getRenderingDevice();
        auto device = renderingDevice.getVirtualDevice();
        commandPool = device.createCommandPool(
                vk::CommandPoolCreateInfo(
                        (vk::CommandPoolCreateFlags) 0,
                        renderingDevice.getGraphics().getIndex()
                )
        );
#ifdef WIN32
        int affinityMask = 1 << index;
        auto returnStauts = SetThreadAffinityMask(thread->native_handle(), affinityMask);
        if (returnStauts == 0) {
            LOG(INFO) << "Unable to set worker " << index << " affinity mask on Win32 (" << GetLastError() << ")";
        } else {
            LOG(INFO) << "Worker " << index << " has affinity mask " << affinityMask << " (Was " << returnStauts << ")";
        }
#endif
    }

    JobWorker::~JobWorker() {
        LOG(INFO) << "Worker " << thread->get_id() << " marked for shutdown";
        running = false;
    }

    bool JobWorker::isAwake() {
        return awaken;
    }

    void JobWorker::awake() {
        {
            std::lock_guard<std::mutex> lock(sleepMutex);
            if (!awaken) {
                awaken = true;
                waiting.notify_one();
            }
        }
    }

    void JobWorker::sleep() {
        {
            std::lock_guard<std::mutex> lock(sleepMutex);
            if (awaken) {
                awaken = false;
            }
        }
        std::unique_lock<std::mutex> lock(handbrakeMutex);
        waiting.wait(lock);
    }

    void JobWorker::workerThread() {
        do {
            Job *jobPtr;
            u32 id;

            while (jobSystem->nextJob(&jobPtr, &id)) {
                jobPtr->operator()(this);
                jobSystem->notifyCompletion(id);
            }
            if (running) {
                sleep();
            }
        } while (running);
        LOG(INFO) << "Worker " << thread->get_id() << " finished execution";
    }

    void JobSystem::notifyCompletion(u32 id) {
        {
            std::lock_guard<std::mutex> guard(graphUsage);
            auto view = tasks.edges_from(id);
            for (std::pair<u32, un::JobDependencyType> edge : view) {
                un::JobDependencyType type = edge.second;
                if (type != un::JobDependencyType::eRequired) {
                    continue;
                }
                u32 dependantID = edge.first;
                bool ready = true;
                tasks.disconnect(id, dependantID);
                tasks.disconnect(dependantID, id);
                for (std::pair<u32, un::JobDependencyType> other : tasks.edges_from(dependantID)) {
                    u32 otherDependency = edge.first;
                    un::JobDependencyType otherType = other.second;
                    if (otherType == un::JobDependencyType::eRequirement) {
                        ready = false;
                    }
                }
                if (ready) {
                    awaitingExecution.push(dependantID);
                }
            }
            tasks.remove(id);
            auto toAwake = awaitingExecution.size();
            int i = 0;
            while (i < workers.size() && toAwake > 0) {
                JobWorker &worker = workers[i++];
                if (!worker.isAwake()) {
                    worker.awake();
                    toAwake--;
                };
            }
        }
    }

    void JobSystem::markForExecution(u32 job) {
        {
            std::lock_guard<std::mutex> lock(queueUsage);
            awaitingExecution.push(job);
            awakeSomeoneUp();
        }
    }

    int JobSystem::getNumWorkers() {
        return workers.size();
    }
}