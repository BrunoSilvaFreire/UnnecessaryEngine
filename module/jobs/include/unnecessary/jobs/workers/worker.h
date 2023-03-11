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
    template<class t_job>
    class job_worker_mixin {
    public:
        using job_type = t_job;
        using job_pointer = job_type*;

    private:
        std::size_t _index;
        u32 _core;
        std::string _name;
        thread _thread;
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
        std::queue<std::pair<job_handle, job_type*>> _pending;
        std::mutex _queueMutex;
        std::mutex _sleepingMutex;
        std::mutex _runningMutex;
        std::condition_variable _jobAvailable;
        thread_safe_event<> _onStarted, _onExited, _onSleeping, _onAwaken;
        thread_safe_event<job_type*, job_handle> _onEnqueued, _onFetched, _onExecuted;

        void worker_thread() {
            _onStarted();
            job_type* jobPtr;
            job_handle id;
            while (next_job(&jobPtr, &id)) {
                _onFetched(jobPtr, id);
                execute(jobPtr, id);
            }
            _onExited();
        }

        bool dequeue(job_type** jobPtr, job_handle* id) {
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

        bool next_job(job_type** jobPtr, job_handle* id) {
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

        void execute(job_type* job, job_handle id) {
            job->operator()(reinterpret_cast<typename job_type::worker_type*>(this));
            _onExecuted(job, id);
            delete job;
        }

        static std::string default_name(size_t index) {
            std::stringstream ss;
            ss << type_name_of<typename t_job::worker_type>() << '#' << to_string(index);
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
        job_worker_mixin(
            std::size_t index,
            std::size_t core
        ) : _index(index),
            _core(static_cast<u32>(core)),
            _name(default_name(index)),
            _thread(
                thread_params(_name, _core),
                std::bind(&job_worker_mixin::worker_thread, this)
            ),
            _running(false),
            _sleeping(false),
            _jobAvailable(),
            _onExited() {
        }

        thread_safe_event<>& get_on_finished() {
            return _onExited;
        }

        void enqueue(job_handle handle, job_type* job) {
            {
                std::lock_guard<std::mutex> lock(_queueMutex);
                _pending.push(std::make_pair(handle, job));
                _onEnqueued(job, handle);
                try_awake();
            }
        }

        void join(fence_notifier<> notifier) {
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
                }
                else {
                    _onSleeping.add_single_fire_listener(
                        [notifier]() mutable {
                            notifier.notify();
                        }
                    );
                }
            }
        }

        u32 get_core() const {
            return _core;
        }

        void set_core(u32 newCore) {
            if (_running) {
                return;
            }
            _core = newCore;
        }

        const std::string& get_name() const {
            return _name;
        }

        void set_name(const std::string& name) {
            if (_running) {
                return;
            }
            _name = name;
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

        void stop(fence_notifier<> notifier) {
            {
                std::lock_guard<std::mutex> lock(_runningMutex);
                if (!_running || _evacuating) {
                    return;
                }

                _onExited.add_single_fire_listener(
                    [notifier]() mutable {
                        notifier.notify();
                    }
                );

                _evacuating = true;
                try_awake();
            }
        }

        bool is_running() const {
            return _running;
        }

        bool is_waiting() const {
            return _sleeping;
        }

        bool is_exiting() const {
            return _evacuating;
        }

        const thread& get_thread() const {
            return _thread;
        }

        size_t get_index() const {
            return _index;
        }

        thread_safe_event<>& on_exited() {
            return _onExited;
        }

        thread_safe_event<>& on_sleeping() {
            return _onSleeping;
        }

        thread_safe_event<>& on_awaken() {
            return _onAwaken;
        }

        thread_safe_event<>& on_started() {
            return _onStarted;
        }

        thread_safe_event<job_type*, job_handle>& on_fetched() {
            return _onFetched;
        }

        thread_safe_event<job_type*, job_handle>& on_executed() {
            return _onExecuted;
        }

        thread_safe_event<job_type*, job_handle>& on_enqueued() {
            return _onEnqueued;
        }
    };
}
#endif
