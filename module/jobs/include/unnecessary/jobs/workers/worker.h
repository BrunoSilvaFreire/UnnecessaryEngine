

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
        un::EventVoid finished;

        void workerThread() {
            JobType* jobPtr;
            JobHandle id;
            while (nextJob(&jobPtr, &id)) {
                execute(jobPtr, id);
            }
            finished();
        }

        bool nextJob(JobType** jobPtr, JobHandle* id) {
            if (provider(jobPtr, id)) {
                return true;
            } else if (exiting) {
                return false;
            }

            std::unique_lock<std::mutex> lock(jobMutex);
            waiting = true;
            jobAvailable.wait(lock);
            waiting = false;
            return running && provider(jobPtr, id);
        }

        void execute(JobType* jobPtr, JobHandle id) {
            jobPtr->operator()(reinterpret_cast<typename JobType::WorkerType*>(this));
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
            finished(),
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
            return finished;
        }

        void notifyJobAvailable() {
            {
                // std::lock_guard<std::mutex> lock(waitingMutex);
                jobAvailable.notify_one();
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