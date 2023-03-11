//
// Created by brunorbsf on 14/12/2021.
//

#ifndef UNNECESSARYENGINE_WORKER_POOL_H
#define UNNECESSARYENGINE_WORKER_POOL_H

#include <unnecessary/jobs/job_graph.h>
#include <unnecessary/jobs/workers/worker.h>
#include <cstring>
#include <condition_variable>
#include <semaphore>

#ifdef WIN32

#include <Windows.h>

#endif
namespace un {
    template<typename T>
    class worker_pool;

    template<typename t_worker>
    struct worker_pool_configuration {
    public:
        using worker_type = t_worker;
        typedef typename worker_type::job_type job_type;
        /**
         * Function that creates workerPools of each archetype during
         * job system initialization.
         */
        typedef std::function<worker_type*(std::size_t)> worker_creator;
    private:
        std::size_t _numWorkers;
        worker_creator _creator;
    public:
        worker_pool_configuration() = default;

        worker_pool_configuration(
            size_t numWorkers,
            const std::function<
                worker_type*(
                    std::size_t
                )
            >& creator
        ) : _numWorkers(numWorkers), _creator(creator) {
        }

        static worker_pool_configuration<worker_type> forwarding(size_t numWorkers) {
            return worker_pool_configuration(
                numWorkers,
                [](
                    std::size_t index
                ) {
                    return new worker_type(index, index, true);
                }
            );
        }

        std::size_t get_num_workers() const {
            return _numWorkers;
        }

        friend class worker_pool<worker_type>;
    };

    /**
     * Holds jobs and workers for a specific archetype.
     * @tparam worker_type The worker archetype inUse for this pool.
     */
    template<typename t_worker>
    class worker_pool {
    public:
        using worker_type = t_worker;
        typedef typename worker_type::job_type job_type;
        typedef typename worker_type::job_pointer job_pointer;
        typedef un::event<job_pointer, job_handle> job_event;
    private:
        struct scheduled_job {
            scheduled_job(
                job_pointer job,
                job_handle graphHandle
            ) : job(job),
                graphHandle(graphHandle) {

            }

            job_pointer job;

            /**
             * The handle of this job in the job system graph
             */
            un::job_handle graphHandle;
        };

        std::size_t _gatlingGun = 0;
        /**
         * Storage for workers.
         */
        std::vector<worker_type*> _workers;
        /**
         * Storage for all jobs scheduled for this pool
         */
        std::vector<scheduled_job> _jobs;
        /**
         * Indices that have been freed and are available to be reused.
         */
        std::queue<std::size_t> _reusableIndices;
        // TODO: Minimize locks
        std::mutex _queueAccessMutex;
        std::mutex _onJobCompletedMutex;
        job_event _onJobCompleted;
        std::mutex _openMutex;
        /**
         * Marks whether this worker pool accepting new jobs.
         */
        bool _open = true;

        std::atomic_size_t _numPendingJobs;
    private:

        void done(job_pointer job, job_handle localHandle) {
            {
                std::lock_guard<std::mutex> lock(_queueAccessMutex);
                _reusableIndices.emplace(static_cast<size_t>(localHandle));
                clear(localHandle);
                _numPendingJobs--;
            }
            {
                std::lock_guard<std::mutex> lock(_onJobCompletedMutex);
                _onJobCompleted(job, localHandle);
            }
        }

        void clear(job_handle handle) {
#if DEBUG
            if (!is_allocated(handle)) {
                throw std::runtime_error("Attempted to clear a handle out of scope");
            }
#endif
            scheduled_job& reference = _jobs[handle];
            std::memset(&reference, 0, sizeof(scheduled_job));
        }

        bool is_allocated(job_handle handle) const {
            return handle < _jobs.size();
        }

        worker_type* create_worker(
            const worker_pool_configuration<worker_type>& configuration,
            size_t index
        ) {
            worker_type* pWorker = configuration._creator(index);
            pWorker->set_core(static_cast<u32>(index));
            std::string threadName = un::type_name_of<worker_type>();
            threadName += "-";
            threadName += std::to_string(index);
            pWorker->set_name(threadName);
            pWorker->on_executed() += [this](job_pointer job, job_handle handle) {
                done(job, handle);
            };
            return pWorker;
        }

        worker_type* next_worker() {
            std::size_t index = _gatlingGun++;
            _gatlingGun %= _workers.size();
            return _workers[index];
        }

    public:

        worker_pool() = default;

        ~worker_pool() {
            stop_all_workers();
            for (worker_type* worker : _workers) {
                delete worker;
            }
        }

        /**
         * Adds the given job to this pool, with the option to immediately add it to the
         * ready jobs list.
         * @param graphHandle The index where the given job is stored inside the job graph.
         * @param dispatch Should this immediately be dispatched.
         * @return The index where the given job is stored inside the pool.
         */
        job_handle enqueue(job_pointer job, job_handle graphHandle, bool dispatch) {
            if (!_open) {
                throw std::runtime_error(
                    "This worker pool is closed and is not accepting new jobs."
                );
            }
            job_handle handle;
            {
                std::lock_guard<std::mutex> lock(_queueAccessMutex);
                if (_reusableIndices.empty()) {
                    handle = static_cast<job_handle>(_jobs.size());
                    _jobs.emplace_back(job, graphHandle);
                }
                else {
                    handle = static_cast<job_handle>(_reusableIndices.front());
                    _reusableIndices.pop();
                    scheduled_job& recycled = _jobs[handle];
                    recycled.job = job;
                    recycled.graphHandle = graphHandle;
                }
                _numPendingJobs++;
                if (dispatch) {
                    unsafe_dispatch(handle);
                }
            }
            return handle;
        }

        job_pointer get_job(un::job_handle localHandle) {
            return _jobs[localHandle].job;
        }

        job_pointer get_job(un::job_handle localHandle) const {
            return _jobs[localHandle].job;
        }

        void dispatch(job_handle jobIndex) {
            {
                std::lock_guard<std::mutex> lock(_queueAccessMutex);
                unsafe_dispatch(jobIndex);
            }
        }

        void dispatch(const std::set<job_handle>& handles) {
            {
                std::lock_guard<std::mutex> lock(_queueAccessMutex);
                for (job_handle handle : handles) {
                    unsafe_dispatch(handle);
                }
            }
        }

        /**
         * Dispatches the handle without locking the queueAccessMutex
         */
        void unsafe_dispatch(job_handle handle) {
            worker_type* worker = next_worker();
            job_pointer pJob = get_job(handle);
#if DEBUG
            if (pJob == nullptr) {
                throw std::runtime_error("Attempted to dispatch invalid handle");
            }
#endif
            worker->enqueue(
                handle,
                pJob
            );
        }

        void allocate_workers(worker_pool_configuration<worker_type> configuration) {
            auto numWorkers = configuration._numWorkers;
            _workers.reserve(numWorkers);
            for (std::size_t i = 0; i < numWorkers; ++i) {
                _workers.emplace_back(create_worker(configuration, i));
            }
        }

        std::vector<worker_type*>& get_workers() {
            return _workers;
        }

        const std::vector<worker_type*>& get_workers() const {
            return _workers;
        }

        std::size_t get_num_workers() const {
            return _workers.size();
        }

        void add_job_completed_listener(const typename job_event::listener& listener) {
            {
                std::lock_guard<std::mutex> lock(_onJobCompletedMutex);
                _onJobCompleted += listener;
            }
        }

        void remove_job_completed_listener(const typename job_event::listener& listener) {
            {
                std::lock_guard<std::mutex> lock(_onJobCompletedMutex);
                _onJobCompleted -= listener;
            }
        }

        const un::event<job_pointer, job_handle>& get_on_job_completed() const {
            return _onJobCompleted;
        }

        void complete() {
            std::lock_guard lock(_openMutex);
            _open = false;
            if (_numPendingJobs == 0) {
                stop_all_workers();
                return;
            }
            add_job_completed_listener(
                [this](job_type* job, un::job_handle handle) {
                    if (_numPendingJobs == 0) {
                        stop_all_workers();
                    }
                }
            );
        }

        void stop_all_workers() {
            un::fences::wait_for(
                _workers.begin(),
                _workers.end(),
                [](worker_type* worker, un::fence_notifier<> notifier) {
                    worker->stop(notifier);
                }
            );
        }

        void start() {
            for (auto worker : get_workers()) {
                worker->start();
            }
        }
    };

}
#endif //UNNECESSARYENGINE_WORKER_POOL_H
