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
    class WorkerPool;

    template<typename TWorker>
    struct WorkerPoolConfiguration {
    public:
        typedef TWorker WorkerType;
        typedef typename WorkerType::JobType JobType;
        /**
         * Function that creates workerPools of each archetype during
         * job system initialization.
         */
        typedef std::function<WorkerType*(std::size_t)> WorkerCreator;
    private:
        std::size_t numWorkers;
        WorkerCreator creator;
    public:
        WorkerPoolConfiguration(
            size_t numWorkers,
            const std::function<TWorker*(
                std::size_t
            )>& creator
        ) : numWorkers(numWorkers), creator(creator) {}

        static WorkerPoolConfiguration<WorkerType> forwarding(
            size_t numWorkers
        ) {
            return WorkerPoolConfiguration(
                numWorkers, [](
                    std::size_t index
                ) {
                    return new WorkerType(index, true);
                }
            );
        }

        std::size_t getNumWorkers() const {
            return numWorkers;
        }

        friend class WorkerPool<WorkerType>;
    };

    /**
     * Holds jobs and workers for a specific archetype.
     * @tparam TWorker The worker archetype inUse for this pool.
     */
    template<typename TWorker>
    class WorkerPool {
    public:
        typedef TWorker WorkerType;
        typedef typename TWorker::JobType JobType;
    private:
        struct ScheduledJob {
            ScheduledJob(
                JobType* job,
                JobHandle graphHandle
            ) : job(job),
                graphHandle(graphHandle) {

            }

            JobType* job;

            /**
             * The handle of this job in the job system graph
             */
            un::JobHandle graphHandle;
        };

        std::size_t gatlingGun = 0;
        /**
         * Storage for workers.
         */
        std::vector<WorkerType*> workers;
        /**
         * Storage for all jobs scheduled for this pool
         */
        std::vector<ScheduledJob> jobs;
        /**
         * Indices that have been freed and are available to be reused.
         */
        std::queue<std::size_t> reusableIndices;
        // TODO: Minimize locks
        std::mutex queueAccessMutex;
        un::Event<JobType*, JobHandle> onJobCompleted;
        std::mutex openMutex;
        /**
         * Marks whether this worker pool accepting new jobs.
         */
        bool open = true;
    private:

        void done(JobType* job, JobHandle localHandle) {
            {
                std::lock_guard<std::mutex> lock(queueAccessMutex);
                reusableIndices.emplace(static_cast<size_t>(localHandle));
                clear(localHandle);
            }
            onJobCompleted(job, localHandle);
        }

        void clear(JobHandle handle) {
#if DEBUG
            if (!isAllocated(handle)) {
                throw std::runtime_error("Attempted to clear a handle out of scope");
            }
#endif
            ScheduledJob& reference = jobs[handle];
            std::memset(&reference, 0, sizeof(ScheduledJob));
        }

        bool isAllocated(JobHandle handle) const { return handle < jobs.size(); }

        WorkerType* createWorker(
            const WorkerPoolConfiguration<WorkerType>& configuration,
            size_t index
        ) {
            WorkerType* pWorker = configuration.creator(index);
            pWorker->setCore(static_cast<u32>(index));
            std::string threadName = un::type_name_of<WorkerType>();
            threadName += "-";
            threadName += std::to_string(index);
            pWorker->setName(threadName);
            pWorker->onExecuted() += [this](JobType* job, JobHandle handle) {
                done(job, handle);
            };
            return pWorker;
        }


        WorkerType* nextWorker() {
            std::size_t index = gatlingGun++;
            gatlingGun %= workers.size();
            return workers[index];
        }

    public:


        WorkerPool() = default;

        ~WorkerPool() {
            for (WorkerType* worker : workers) {
                worker->stop(true);
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
        JobHandle enqueue(JobType* job, JobHandle graphHandle, bool dispatch) {
            if (!open) {
                throw std::runtime_error(
                    "This worker pool is closed and is not accepting new jobs."
                );
            }
            JobHandle handle;
            {
                std::lock_guard<std::mutex> lock(queueAccessMutex);
                if (reusableIndices.empty()) {
                    handle = static_cast<JobHandle>(jobs.size());
                    jobs.emplace_back(job, graphHandle);
                } else {
                    handle = static_cast<JobHandle>(reusableIndices.front());
                    reusableIndices.pop();
                    ScheduledJob& recycled = jobs[handle];
                    recycled.job = job;
                    recycled.graphHandle = graphHandle;
                }
                if (dispatch) {
                    unsafeDispatch(handle);
                }
            }
            return handle;
        }

        un::Job<WorkerType>* getJob(un::JobHandle localHandle) {
            return jobs[localHandle].job;
        }

        void dispatch(JobHandle jobIndex) {
            {
                std::lock_guard<std::mutex> lock(queueAccessMutex);
                unsafeDispatch(jobIndex);
            }
        }

        void dispatch(const std::set<JobHandle>& handles) {
            {
                std::lock_guard<std::mutex> lock(queueAccessMutex);
                for (JobHandle handle : handles) {
                    unsafeDispatch(handle);
                }
            }
        }

        /**
         * Dispatches the handle without locking the queueAccessMutex
         */
        void unsafeDispatch(JobHandle handle) {
            WorkerType* worker = nextWorker();
            JobType* pJob = getJob(handle);
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


        void allocateWorkers(WorkerPoolConfiguration<WorkerType> configuration) {
            auto numWorkers = configuration.numWorkers;
            workers.reserve(numWorkers);
            for (std::size_t i = 0; i < numWorkers; ++i) {
                workers.emplace_back(createWorker(configuration, i));
            }
        }

        std::vector<WorkerType*>& getWorkers() {
            return workers;
        }

        const std::vector<WorkerType*>& getWorkers() const {
            return workers;
        }

        std::atomic_size_t getNumWorkers() const {
            return workers.size();
        }

        un::Event<JobType*, JobHandle>& getOnJobCompleted() {
            return onJobCompleted;
        }

        const un::Event<JobType*, JobHandle>& getOnJobCompleted() const {
            return onJobCompleted;
        }

        void complete() {
            std::lock_guard lock(openMutex);
            open = false;
            completeAllWorkers();
        }

        void completeAllWorkers() {
            for (auto worker : workers) {
                worker->stop(true);
            }
        }

        void start() {
            for (auto worker : getWorkers()) {
                worker->start();
            }
        }
    };

}
#endif //UNNECESSARYENGINE_WORKER_POOL_H
