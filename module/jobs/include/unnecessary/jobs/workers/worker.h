

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
        un::Thread* _thread;
        /**
         * Is this worker still processing it's tasks or should it exit?
         */
        bool _running = false;
        /**
         * Is this worker currently idle, waiting for jobs?
         */
        bool _waiting = false;
        /**
         * Has this worker been ordered to immediately stop?
         */
        bool _evacuating = false;
        std::queue<std::pair<un::JobHandle, JobType*>> _pending;
        std::mutex _queueMutex;
        std::mutex _sleepingMutex;
        std::mutex _runningMutex;
        std::condition_variable _jobAvailable;
        un::EventVoid _started, _exited, _sleeping, _awaken;
        un::Event<JobType*, JobHandle> _enqueued, _fetched, _executed;

        void workerThread() {
            _started();
            JobType* jobPtr;
            JobHandle id;
            while (nextJob(&jobPtr, &id)) {
                _fetched(jobPtr, id);
                execute(jobPtr, id);
            }
            _exited();
        }

        bool tryDequeue(JobType** jobPtr, JobHandle* id) {
            std::unique_lock<std::mutex> lock(_queueMutex);
            bool hasJobs = !_pending.empty();
            if (hasJobs) {
                const auto& pair = _pending.front();
                *id = pair.first;
                *jobPtr = pair.second;
//                LOG(INFO) << "Popped " << _index << ", _pending: " << _pending.size();
                _pending.pop();
                return true;
            } else {
                std::lock_guard<std::mutex> _runningLock(_runningMutex);
                if (_evacuating) {
                    return false;
                }
            }
            return false;
        }

        bool canDequeue() {
            std::lock_guard<std::mutex> lock(_runningMutex);
            return !_evacuating && _running;
        }

        bool nextJob(JobType** jobPtr, JobHandle* id) {
            if (!canDequeue()) {
                return false;
            }
            if (tryDequeue(jobPtr, id)) {
                return true;
            }
            {
                std::unique_lock<std::mutex> sLock(_sleepingMutex);
                _waiting = true;
                _sleeping();
                _jobAvailable.wait(sLock);
                _waiting = false;
                _awaken();
            }

            return canDequeue() && tryDequeue(jobPtr, id);
        }

        void execute(JobType* jobPtr, JobHandle id) {
            jobPtr->operator()(reinterpret_cast<typename JobType::WorkerType*>(this));
            _executed(jobPtr, id);
            delete jobPtr;
        }

    public:
        JobWorkerMixin(
            size_t _index,
            bool autostart
        ) : _index(_index),
            _thread(nullptr),
            _jobAvailable(),
            _running(false),
            _waiting(false),
            _core(-1),
            _exited() {
            if (autostart) {
                start();
            }
        }

        virtual ~JobWorkerMixin() {
            if (_thread != nullptr) {
                _thread->join();
                delete _thread;
            }
        }

        bool hasPending() const {
            return !_pending.empty();
        }

        bool isAwake() const {
            return !_waiting;
        }

        bool tryAwake() {
            if (isSleeping()) {
//                LOG(INFO) << "Awaking " << _index;
                _jobAvailable.notify_one();
                return true;
            }
            return false;
        }

        bool isSleeping() {
            std::lock_guard<std::mutex> lock(_sleepingMutex);
            return _waiting;
        }

        un::EventVoid& getOnFinished() {
            return _exited;
        }

        void enqueue(un::JobHandle handle, JobType* job) {
            {
                std::lock_guard<std::mutex> lock(_queueMutex);
                _pending.push(std::make_pair(handle, job));
                _enqueued(job, handle);
                tryAwake();
            }
        }

        u32 getCore() const {
            return _core;
        }

        void join(std::size_t handle, un::Fence<>& fence) {
            {
                std::lock_guard<std::mutex> lock(_sleepingMutex);
                if (_waiting) {
                    fence.notify(handle);
                } else {
                    _sleeping.addSingleFireListener(
                        [&fence, handle]() {
                            fence.notify(handle);
                        }
                    );
                }
            }
        }

        void setCore(u32 newCore) {
            JobWorkerMixin::_core = newCore;
            if (_running) {
                _thread->setCore(newCore);
            }
        }

        const std::string& getName() const {
            return _name;
        }

        void setName(const std::string& name) {
            JobWorkerMixin::_name = name;
            if (_running) {
                _thread->setName(name);
            }
        }

        void start() {
            {
                std::lock_guard<std::mutex> lock(_runningMutex);
                if (_running) {
                    return;
                }

                _running = true;
                _thread = new un::Thread(
                    std::bind(
                        &JobWorkerMixin::workerThread,
                        this
                    )
                );
                _thread->setCore(_core);
                _thread->setName(_name);
                _thread->start();
            }
        }

        void stop() {
            {
                std::lock_guard<std::mutex> lock(_runningMutex);
                if (!_running) {
                    return;
                }
                _evacuating = true;
                tryAwake();
            }
        }

        bool isRunning() const {
            return _running;
        }

        bool isWaiting() const {
            return _waiting;
        }

        bool isExiting() const {
            return _evacuating;
        }

        un::Thread* getThread() const {
            return _thread;
        }

        size_t getIndex() const {
            return _index;
        }

        EventVoid& onExited() {
            return _exited;
        }

        EventVoid& onSleeping() {
            return _sleeping;
        }

        EventVoid& onAwaken() {
            return _awaken;
        }

        EventVoid& onStarted() {
            return _started;
        }

        un::Event<JobType*, JobHandle>& onFetched() {
            return _fetched;
        }

        un::Event<JobType*, JobHandle>& onExecuted() {
            return _executed;
        }

        un::Event<JobType*, JobHandle>& onEnqueued() {
            return _enqueued;
        }

    };

}
#endif