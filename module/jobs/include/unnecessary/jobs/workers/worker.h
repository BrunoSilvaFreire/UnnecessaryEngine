

#ifndef UNNECESSARYENGINE_WORKER_H
#define UNNECESSARYENGINE_WORKER_H

#include <thread>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <unnecessary/jobs/job.h>
#include <unnecessary/jobs/thread.h>
#include <unnecessary/jobs/misc/fence.h>
#include <unnecessary/logging.h>
#include <unnecessary/misc/event.h>
#include <unnecessary/misc/types.h>

namespace un {

    template<class TJob>
    class JobWorkerMixin {
    public:
        typedef TJob JobType;
        typedef JobType* JobPointer;
    private:
        std::size_t _index;
        u32 _core;
        std::string _name;
        un::Thread _thread;
        /**
         * Is this worker still processing it's tasks or should it exit?
         */
        bool _running = false;
        /**
         * Is this worker currently idle, waiting for jobs?
         */
        bool _sleeping = false;
        /**
         * Has this worker been ordered to immediately stop?
         */
        bool _evacuating = false;
        std::queue<std::pair<un::JobHandle, JobType*>> _pending;
        std::mutex _queueMutex;
        std::mutex _sleepingMutex;
        std::mutex _runningMutex;
        std::condition_variable _jobAvailable;
        un::ThreadSafeEvent<> _onStarted, _onExited, _onSleeping, _onAwaken;
        un::ThreadSafeEvent<JobType*, JobHandle> _onEnqueued, _onFetched, _onExecuted;

        void worker_thread() {
            _onStarted();
            JobType* jobPtr;
            JobHandle id;
            while (next_job(&jobPtr, &id)) {
                _onFetched(jobPtr, id);
                execute(jobPtr, id);
            }
            _onExited();
        }

        bool dequeue(JobType** jobPtr, JobHandle* id) {
            std::unique_lock<std::mutex> lock(_queueMutex);
            bool hasJobs = !_pending.empty();
            if (hasJobs) {
                const auto& pair = _pending.front();
                *id = pair.first;
                *jobPtr = pair.second;
                _pending.pop();
                return true;
            }
            return false;
        }

        bool next_job(JobType** jobPtr, JobHandle* id) {
            {
                std::lock_guard<std::mutex> lock(_runningMutex);
                if (_evacuating || !_running) {
                    return false;
                }
            }

            if (dequeue(jobPtr, id)) {
                // There is another job ready
                return true;
            }

            // We're empty, go to sleep until a job is available
            {
                std::unique_lock<std::mutex> sLock(_sleepingMutex);
                _sleeping = true;
                _onSleeping();
                _jobAvailable.wait(sLock);
                _sleeping = false;
                _onAwaken();
            }

            // We were woken up, try again
            return next_job(jobPtr, id);
        }

        void execute(JobType* job, JobHandle id) {
            job->operator()(reinterpret_cast<typename JobType::WorkerType*>(this));
            _onExecuted(job, id);
            delete job;
        }

        static std::string default_name(size_t index) {
            std::stringstream ss;
            ss << type_name_of<typename TJob::WorkerType>() << '#' << to_string(index);
            return ss.str();
        }


        bool try_awake() {
            {
                std::lock_guard<std::mutex> lock(_sleepingMutex);
                if (_sleeping) {
                    _jobAvailable.notify_one();
                    return true;
                }
            }
            return false;
        }

    public:
        JobWorkerMixin(
            std::size_t index,
            std::size_t core
        ) : _index(index),
            _jobAvailable(),
            _running(false),
            _sleeping(false),
            _core(static_cast<u32>(core)),
            _onExited(),
            _name(default_name(index)),
            _thread(
                un::ThreadParams(_name, _core),
                std::bind(&JobWorkerMixin::worker_thread, this)
            ) {
        }

        un::ThreadSafeEvent<>& getOnFinished() {
            return _onExited;
        }

        void enqueue(un::JobHandle handle, JobType* job) {
            {
                std::lock_guard<std::mutex> lock(_queueMutex);
                _pending.push(std::make_pair(handle, job));
                _onEnqueued(job, handle);
                try_awake();
            }
        }

        void join(un::FenceNotifier<> notifier) {
            {
                std::lock_guard<std::mutex> lock(_runningMutex);
                if (_evacuating || !_running) {
                    notifier.notify();
                    return;
                }
            }

            {
                std::lock_guard<std::mutex> lock(_sleepingMutex);
                if (_sleeping) {
                    notifier.notify();
                } else {
                    _onSleeping.addSingleFireListener(
                        [notifier]() mutable {
                            notifier.notify();
                        }
                    );
                }
            }
        }

        u32 getCore() const {
            return _core;
        }

        void setCore(u32 newCore) {
            if (_running) {
                return;
            }
            JobWorkerMixin::_core = newCore;
        }

        const std::string& getName() const {
            return _name;
        }

        void setName(const std::string& name) {
            if (_running) {
                return;
            }
            JobWorkerMixin::_name = name;

        }

        void start() {
            {
                std::lock_guard<std::mutex> lock(_runningMutex);
                if (_running) {
                    return;
                }

                _running = true;
                _thread.start();
            }
        }

        void stop(un::FenceNotifier<> notifier) {
            {
                std::lock_guard<std::mutex> lock(_runningMutex);
                if (!_running || _evacuating) {
                    return;
                }

                _onExited.addSingleFireListener(
                    [notifier]() mutable {
                        notifier.notify();
                    }
                );

                _evacuating = true;
                try_awake();
            }
        }

        bool isRunning() const {
            return _running;
        }

        bool isWaiting() const {
            return _sleeping;
        }

        bool isExiting() const {
            return _evacuating;
        }

        const un::Thread& getThread() const {
            return _thread;
        }

        size_t getIndex() const {
            return _index;
        }

        un::ThreadSafeEvent<>& onExited() {
            return _onExited;
        }

        un::ThreadSafeEvent<>& onSleeping() {
            return _onSleeping;
        }

        un::ThreadSafeEvent<>& onAwaken() {
            return _onAwaken;
        }

        un::ThreadSafeEvent<>& onStarted() {
            return _onStarted;
        }

        un::ThreadSafeEvent<JobType*, JobHandle>& onFetched() {
            return _onFetched;
        }

        un::ThreadSafeEvent<JobType*, JobHandle>& onExecuted() {
            return _onExecuted;
        }

        un::ThreadSafeEvent<JobType*, JobHandle>& onEnqueued() {
            return _onEnqueued;
        }

    };

}
#endif