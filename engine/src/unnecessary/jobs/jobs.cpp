#include <unnecessary/jobs/jobs.h>

#include <utility>

#ifdef WIN32

#include <windows.h>

#endif
namespace un {

    LambdaJob::operator void() {
        callback();
    }

    LambdaJob::LambdaJob(LambdaJob::Callback callback) : callback(std::move(callback)) {}

    u32 JobSystem::enqueue(Job *job) {
        return enqueue(job, true);
    }

    u32 JobSystem::enqueue(Job *job, bool alsoMarkForExecution) {
        u32 id;
        {
            std::lock_guard<std::mutex> guard(graphUsage);
            id = tasks.addVertex(job);
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
            tasks.connect(to, from, un::JobDependencyType::eRequirement);
            tasks.connect(from, to, un::JobDependencyType::eRequired);
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
        for (JobWorker &worker : threads) {
            if (!worker.isAwake()) {
                // Wake up worker
                worker.awake();
                break;
            }
        }
    }

    JobSystem::JobSystem() : queueUsage() {
        auto nCores = std::thread::hardware_concurrency();
        if (nCores == 0) {
            nCores = 4;
        }
        LOG(INFO) << "Using " << GREEN(nCores) << " workers for job system.";
        threads.reserve(nCores);
        for (int i = 0; i < nCores; ++i) {
            threads.emplace_back(this);
        }
    }

    JobSystem::~JobSystem() = default;

    u32 JobSystem::enqueue(un::LambdaJob::Callback callback) {
        return enqueue(new LambdaJob(std::move(callback)));
    }

    Job *JobSystem::getJob(u32 id) {
        return tasks[id];
    }

    JobSystem::JobWorker::JobWorker(
            JobSystem::JobWorker &&copy
    ) noexcept: thread(copy.thread),
                jobSystem(copy.jobSystem),
                running(copy.running),
                waiting() {
    }

    JobSystem::JobWorker::JobWorker(
            JobSystem *jobSystem
    ) : thread(new std::thread(&JobWorker::workerThread, this)),
        jobSystem(jobSystem),
        waiting(),
        running(true) {

    }

    JobSystem::JobWorker::~JobWorker() {
        LOG(INFO) << "Worker " << thread->get_id() << " marked for shutdown";
        running = false;
    }

    bool JobSystem::JobWorker::isAwake() {
        return awaken;
    }

    void JobSystem::JobWorker::awake() {
        if (!awaken) {
            awaken = true;
            waiting.notify_one();
        }
    }

    void JobSystem::JobWorker::sleep() {
        if (awaken) {
            awaken = false;
            std::unique_lock<std::mutex> lock(mutex);
            waiting.wait(lock);
        }
    }

    void JobSystem::JobWorker::workerThread() {
        do {
            Job *jobPtr;
            u32 id;
            while (jobSystem->nextJob(&jobPtr, &id)) {
                jobPtr->operator void();
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
            for (std::tuple<u32, un::JobDependencyType> edge : tasks.edgesFrom(id)) {
                un::JobDependencyType type = std::get<1>(edge);
                if (type != un::JobDependencyType::eRequirement) {
                    continue;
                }
                u32 dependantID = std::get<0>(edge);
                bool ready = true;
                tasks.disconnect(id, dependantID);
                for (std::tuple<u32, un::JobDependencyType> other : tasks.edgesFrom(dependantID)) {
                    un::JobDependencyType otherType = std::get<1>(edge);
                    if (otherType == un::JobDependencyType::eRequirement) {
                        ready = false;
                    }
                }
                if (ready) {
                    awaitingExecution.push(dependantID);
                }
            }
            tasks.removeVertex(id);
        }
    }

    void JobSystem::markForExecution(u32 job) {
        awaitingExecution.push(job);
        awakeSomeoneUp();
    }
}