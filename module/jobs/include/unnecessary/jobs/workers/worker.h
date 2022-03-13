

#ifndef UNNECESSARYENGINE_WORKER_H
#define UNNECESSARYENGINE_WORKER_H

#include <thread>
#include <condition_variable>
#include <mutex>
#include <unnecessary/jobs/job.h>
#include <unnecessary/jobs/thread.h>
#include <unnecessary/logging.h>
#include <unnecessary/misc/event.h>
#include <unnecessary/misc/types.h>

namespace un {

    static const size_t arrLen = 512;

    template<class _Job>
    class AbstractJobWorker {
    public:
        typedef _Job JobType;
    private:
        size_t index;
        u32 core;
        std::string name;
        un::Thread* thread;
        un::JobProvider<JobType> provider;
        un::JobNotifier<JobType> notifier;
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
        std::mutex jobMutex;
        std::mutex runningMutex;
        std::condition_variable jobAvailable;
        un::EventVoid started, exited, sleeping, awaken;
        un::Event<JobType*, JobHandle> fetched, executed;

        void workerThread() {
            started();
            JobType* jobPtr;
            JobHandle id;
            while (nextJob(&jobPtr, &id)) {
                if (jobPtr == nullptr){
                    continue;
                }
                fetched(jobPtr, id);
                execute(jobPtr, id);
            }
            exited();
        }

        bool nextJob(JobType** jobPtr, JobHandle* id) {
            if (provider(jobPtr, id)) {
                return true;
            } else if (exiting) {
                return false;
            }

            std::unique_lock<std::mutex> lock(jobMutex);
            waiting = true;
            sleeping();
            jobAvailable.wait(lock);
            waiting = false;
            awaken();
            return running && provider(jobPtr, id);
        }

        void execute(JobType* jobPtr, JobHandle id) {
            jobPtr->operator()(reinterpret_cast<typename JobType::WorkerType*>(this));
            executed(jobPtr, id);
            notifier(jobPtr, id);
        }

    public:
        AbstractJobWorker(
            size_t index,
            bool autostart,
            un::JobProvider<JobType> provider,
            un::JobNotifier<JobType> notifier
        ) : index(index),
            thread(nullptr),
            jobAvailable(),
            running(false),
            waiting(false),
            exited(),
            provider(provider),
            notifier(notifier) {
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

        bool isSleeping() const {
            return waiting;
        }

        un::EventVoid& getOnFinished() {
            return exited;
        }

        void notifyJobAvailable() {
            {
                // std::lock_guard<std::mutex> lock(waitingMutex);
                jobAvailable.notify_one();
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
                notifyJobAvailable();
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
            bool autostart,
            const JobProvider<JobType>& provider,
            const JobNotifier<JobType>& notifier
        );
    };
}
#endif