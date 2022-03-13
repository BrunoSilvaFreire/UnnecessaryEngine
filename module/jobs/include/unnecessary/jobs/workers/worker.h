

#ifndef UNNECESSARYENGINE_WORKER_H
#define UNNECESSARYENGINE_WORKER_H

#include <thread>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <unnecessary/jobs/job.h>
#include <unnecessary/jobs/thread.h>
#include <unnecessary/logging.h>
#include <unnecessary/misc/event.h>
#include <unnecessary/misc/types.h>

namespace un {

    template<class TJob>
    class AbstractJobWorker {
    public:
        typedef TJob JobType;
    private:
        size_t index;
        u32 core;
        std::string name;
        un::Thread* thread;
        /**
         * Is this worker still processing it's tasks or should it exit?
         */
        bool running = false;
        /**
         * Is this worker complete all of them and is currently sleeping while waiting for more?
         */
        bool waiting = false;

        /**
         * Has this worker been ordered to stop?
         */
        bool exiting = false;

        std::queue<std::pair<un::JobHandle, JobType*>> pending;
        std::mutex queueMutex;
        std::mutex sleepingMutex;
        std::mutex runningMutex;
        std::condition_variable jobAvailable;
        un::EventVoid started, exited, sleeping, awaken;
        un::Event<JobType*, JobHandle> fetched, executed;

        void workerThread() {
            started();
            JobType* jobPtr;
            JobHandle id;
            while (nextJob(&jobPtr, &id)) {
                fetched(jobPtr, id);
                execute(jobPtr, id);
            }
            exited();
        }

        bool tryDequeue(JobType** jobPtr, JobHandle* id) {
            bool hasJobs = !pending.empty();
            if (hasJobs) {
                const auto& pair = pending.front();
                *id = pair.first;
                *jobPtr = pair.second;
//                LOG(INFO) << "Popped " << index << ", pending: " << pending.size();
                pending.pop();
                return true;
            } else if (exiting) {
                return false;
            }
            return false;
        }

        bool nextJob(JobType** jobPtr, JobHandle* id) {
            std::unique_lock<std::mutex> lock(queueMutex);
//            LOG(INFO) << "Fetching " << index;
            if (tryDequeue(jobPtr, id)) {
                return true;
            } else if (exiting) {
                return false;
            }

            {
                std::lock_guard<std::mutex> sLock(sleepingMutex);
                waiting = true;
                sleeping();
            }
//            LOG(INFO) << "Sleeping " << index << " @ " << pending.size();
            jobAvailable.wait(lock);
            {
                std::lock_guard<std::mutex> sLock(sleepingMutex);
                waiting = false;
                awaken();
            }

            return running && tryDequeue(jobPtr, id);
        }

        void execute(JobType* jobPtr, JobHandle id) {
            jobPtr->operator()(reinterpret_cast<typename JobType::WorkerType*>(this));
            executed(jobPtr, id);
//            LOG(INFO) << "Executed @ " << index << ", pending: " << pending.size();
            delete jobPtr;
        }

    public:
        AbstractJobWorker(
            size_t index,
            bool autostart
        ) : index(index),
            thread(nullptr),
            jobAvailable(),
            running(false),
            waiting(false),
            exited() {
            if (autostart) {
                start();
            }
        }

        virtual ~AbstractJobWorker() {
            if (thread != nullptr) {
                thread->join();
                delete thread;
            }
        }

        bool isAwake() const {
            return !waiting;
        }

        bool tryAwake() {
            if (isSleeping()) {
//                LOG(INFO) << "Awaking " << index;
                jobAvailable.notify_one();
                return true;
            }
            return false;
        }

        bool isSleeping() {
            std::lock_guard<std::mutex> lock(sleepingMutex);
            return waiting;
        }

        un::EventVoid& getOnFinished() {
            return exited;
        }

        void enqueue(un::JobHandle handle, JobType* job) {
            {
                std::lock_guard<std::mutex> lock(queueMutex);
                pending.push(std::make_pair(handle, job));
//                LOG(INFO) << "Enqueued " << handle << " @ " << index << ", " << pending.size();
                tryAwake();
            }
        }

        u32 getCore() const {
            return core;
        }

        void setCore(u32 core) {
            AbstractJobWorker::core = core;
            if (running) {
                thread->setCore(core);
            }
        }

        const std::string& getName() const {
            return name;
        }

        void setName(const std::string& name) {
            AbstractJobWorker::name = name;
            if (running) {
                thread->setName(name);
            }
        }

        void start() {
            {
                std::lock_guard<std::mutex> lock(runningMutex);
                if (running) {
                    return;
                }

                running = true;
                thread = new un::Thread(
                    std::bind(
                        &AbstractJobWorker::workerThread,
                        this
                    )
                );
                thread->setCore(core);
                thread->setName(name);
            }
        }

        void stop(bool completelyConsume) {
            {
                std::lock_guard<std::mutex> lock(runningMutex);
                if (!running) {
                    return;
                }
                exiting = completelyConsume;
                tryAwake();
            }
        }

        bool isRunning() const {
            return running;
        }

        bool isWaiting() const {
            return waiting;
        }

        bool isExiting() const {
            return exiting;
        }

        un::Thread* getThread() const {
            return thread;
        }

        size_t getIndex() const {
            return index;
        }

        EventVoid& onExited() {
            return exited;
        }

        EventVoid& onSleeping() {
            return sleeping;
        }

        EventVoid& onAwaken() {
            return awaken;
        }

        EventVoid& onStarted() {
            return started;
        }

        un::Event<JobType*, JobHandle>& onFetched() {
            return fetched;
        }

        un::Event<JobType*, JobHandle>& onExecuted() {
            return executed;
        }

    };

    class JobWorker;

    typedef un::Job<JobWorker> SimpleJob;

    class JobWorker : public un::AbstractJobWorker<un::SimpleJob> {
    public:
        JobWorker(
            size_t index,
            bool autostart
        );
    };
}
#endif