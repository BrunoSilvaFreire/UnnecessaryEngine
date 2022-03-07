//
// Created by brunorbsf on 14/12/2021.
//

#ifndef UNNECESSARYENGINE_WORKER_POOL_H
#define UNNECESSARYENGINE_WORKER_POOL_H

#include <unnecessary/jobs/job_graph.h>
#include <unnecessary/jobs/workers/worker.h>
#include <condition_variable>
#include <semaphore>

#ifdef WIN32

#include <Windows.h>

#endif
namespace un {
    template<typename T>
    class WorkerPool;

    template<typename _WorkerType>
    struct WorkerArchetypeConfiguration {
    public:
        typedef _WorkerType WorkerType;
        typedef typename WorkerType::JobType JobType;
        /**
         * Function that creates workerPools of each archetype during
         * job system initialization.
         */
        typedef std::function<
            WorkerType*(
                std::size_t,
                un::JobProvider<JobType>,
                un::JobNotifier<JobType>
            )
        > WorkerCreator;
    private:
        std::size_t numWorkers;
        WorkerCreator creator;
    public:
        WorkerArchetypeConfiguration(
            size_t numWorkers,
            const std::function<_WorkerType*(
                std::size_t,
                un::JobProvider<JobType>,
                un::JobNotifier<JobType>
            )>& creator
        ) : numWorkers(numWorkers), creator(creator) {}

        static WorkerArchetypeConfiguration<WorkerType> forwarding(
            size_t numWorkers
        ) {
            return WorkerArchetypeConfiguration(
                numWorkers, [](
                    std::size_t index,
                    un::JobProvider<typename WorkerType::JobType> provider,
                    un::JobNotifier<typename WorkerType::JobType> notifier
                ) {
                    return new WorkerType(index, true, provider, notifier);
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

        /**
         * Storage for workers.
         */
        std::vector<WorkerType*> workers;
        /**
         * Storage for all jobs scheduled for this pool
         */
        std::vector<ScheduledJob> jobs;
        /**
         * Jobs that are ready to be executed by this pool.
         */
        std::queue<JobHandle> ready;
        /**
         * Jobs that are pending execution.
         */
        std::set<un::JobHandle> pending;
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

        bool tryRetrieveJob(JobType** job, JobHandle* handle) {
            {
                std::lock_guard<std::mutex> lock(queueAccessMutex);
                if (!ready.empty()) {
                    JobHandle next = ready.front();
                    ready.pop();
                    const auto& reference = jobs[next];
                    *job = reference.job;
                    *handle = reference.graphHandle;
                    return true;
                }
            }
            return false;
        }

        void done(JobType* job, JobHandle localHandle) {
            {
                std::lock_guard<std::mutex> lock(queueAccessMutex);
                reusableIndices.push(static_cast<size_t>(localHandle));
                pending.erase(localHandle);
            }
            clear(localHandle);
            onJobCompleted(job, localHandle);
            delete job;
        }

        void clear(JobHandle handle) {
            ScheduledJob& reference = jobs[handle];
            reference.job = nullptr;
            reference.graphHandle = std::numeric_limits<un::JobHandle>::max();
        }

        WorkerType* createWorker(
            const WorkerArchetypeConfiguration<WorkerType>& configuration,
            size_t index
        ) {
            WorkerType* pWorker = configuration.creator(
                index,
                [&, this](JobType** pJob, JobHandle* pId) {
                    return tryRetrieveJob(pJob, pId);
                },
                [&, this](JobType* job, JobHandle id) {
                    done(job, id);
                }
            );
            un::Thread* thread = pWorker->getThread();

            if (!thread->setCore(index)) {
                LOG(WARN) << "Unable to set worker " << index << " to it's respective core";
            }
            std::string threadName = un::type_name_of<WorkerType>();
            threadName += "-";
            threadName += std::to_string(index);
            thread->setName(threadName);
            return pWorker;
        }

        size_t ensureNumWorkersAwake(size_t num) {
            size_t numWorkersNeededToAwake = num;
            std::size_t lastIndex = workers.size();
            for (size_t i = 0; i < lastIndex; ++i) {
                const auto& worker = workers[i];
                if (worker->isSleeping()) {
                    worker->notifyJobAvailable();
                    numWorkersNeededToAwake--;
                }
                if (numWorkersNeededToAwake == 0) {
                    return 0;
                }
            }
            return numWorkersNeededToAwake;
        }


    public:

        WorkerPool(JobGraph* graph) : queueAccessMutex() {

        }

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
                    handle = jobs.size();
                    jobs.emplace_back(job, graphHandle);
                } else {
                    handle = reusableIndices.front();
                    reusableIndices.pop();
                    ScheduledJob& recycled = jobs[handle];
                    recycled.job = job;
                    recycled.graphHandle = graphHandle;
                }
                if (dispatch) {
                    unsafeDispatch(handle);
                }
                pending.emplace(handle);
            }
            return handle;
        }

        void dispatch(JobHandle jobIndex) {
            {
                std::lock_guard<std::mutex> lock(queueAccessMutex);
                unsafeDispatch(jobIndex);
            }
        }

        void dispatch(std::set<JobHandle> handles) {
            {
                std::lock_guard<std::mutex> lock(queueAccessMutex);
                for (JobHandle handle : handles) {
                    ready.push(handle);
                }
                ensureNumWorkersAwake(ready.size());
            }
        }

        /**
         * Dispatches the handle without locking the queueAccessMutex
         */
        void unsafeDispatch(JobHandle handle) {
            ready.push(handle);
            ensureNumWorkersAwake(ready.size());
        }


        void allocateWorkers(WorkerArchetypeConfiguration<WorkerType> configuration) {
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

        bool hasPendingJobs() {
            return !pending.empty();
        }

        void complete() {
            std::lock_guard lock(openMutex);
            open = false;
            if (hasPendingJobs()) {
                completeAllWorkers();
            }
        }

        void completeAllWorkers() {
            for (auto worker : workers) {
                worker->stop(true);
            }
        }
    };

}
#endif //UNNECESSARYENGINE_WORKER_POOL_H
