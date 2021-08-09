#include <unnecessary/application.h>
#include <unnecessary/jobs/jobs.h>

#include <utility>

#ifdef WIN32

#include <windows.h>

#endif
namespace un {

    void LambdaJob::operator()(un::JobWorker *worker) {
        if (callback != nullptr) {
            callback(worker);
        }
        if (voidCallback != nullptr) {
            voidCallback();
        }
    }

    LambdaJob::LambdaJob(
            const Callback &callback
    ) : callback(callback), voidCallback(nullptr) {}

    LambdaJob::LambdaJob(
            const VoidCallback &callback
    ) : voidCallback(callback), callback() {

    }

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

    u32 JobSystem::enqueue(u32 dependsOn, const un::LambdaJob::Callback &callback) {
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

    JobSystem::JobSystem(un::Application &application, int nThreads) : queueUsage(), graphUsage() {
        size_t nCores;
        if (nThreads <= 0) {
            nCores = std::thread::hardware_concurrency();
        } else {
            nCores = nThreads;
        }
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
                waiting(),
                index(copy.index),
                graphicsResources(copy.graphicsResources) {
    }

    JobWorker::JobWorker(
            un::Application &application,
            JobSystem *jobSystem,
            size_t index
    ) : thread(new std::thread(&JobWorker::workerThread, this)),
        jobSystem(jobSystem),
        waiting(),
        running(true),
        awaken(false),
        index(index),
        graphicsResources(application.getRenderer().getRenderingDevice()) {
#ifdef MSVC
        int affinityMask = 1 << index;
        HANDLE hThread = reinterpret_cast<HANDLE>(thread->native_handle());
        auto returnStauts = SetThreadAffinityMask(hThread, affinityMask);
        if (returnStauts == 0) {
            LOG(INFO) << "Unable to set worker " << index << " affinity mask on Win32 (" << GetLastError() << ")";
        } else {
            LOG(INFO) << "Worker " << index << " has affinity mask " << affinityMask << " (Was " << returnStauts << ")";
        }
        std::string threadName = "UnnecessaryWorker-";
        threadName += std::to_string(index);
        SetThreadDescription(hThread, std::wstring(threadName.begin(), threadName.end()).c_str());
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

    const JobWorker::WorkerGraphicsResources &JobWorker::getGraphicsResources() const {
        return graphicsResources;
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

    JobWorker::WorkerGraphicsResources::WorkerGraphicsResources(RenderingDevice &renderingDevice) {

        auto device = renderingDevice.getVirtualDevice();
        commandPool = device.createCommandPool(
                vk::CommandPoolCreateInfo(
                        (vk::CommandPoolCreateFlags) vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
                        renderingDevice.getGraphics().getIndex()
                )
        );

    }

    const vk::CommandPool &JobWorker::WorkerGraphicsResources::getCommandPool() const {
        return commandPool;
    }
}