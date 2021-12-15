//
// Created by brunorbsf on 14/12/2021.
//

#ifndef UNNECESSARYENGINE_WORKER_POOL_H
#define UNNECESSARYENGINE_WORKER_POOL_H

#include <unnecessary/jobs/job_graph.h>
#include <unnecessary/jobs/workers/worker.h>
#include <condition_variable>
#include <semaphore>

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

        std::size_t getNumWorkers() const {
            return numWorkers;
        }

        friend class WorkerPool<WorkerType>;
    };

    /**
     * Holds jobs and workers for a specific archetype.
     * @tparam _WorkerType The worker archetype used for this pool.
     */
    template<typename _WorkerType>
    class WorkerPool {
    public:
        typedef _WorkerType WorkerType;
        typedef typename _WorkerType::JobType JobType;
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
         * Indices that have been freed and are available to be reused.
         */
        std::queue<std::size_t> reusableIndices;
        // TODO: Minimize locks
        std::mutex queueAccessMutex;
        un::Event<JobType*, JobHandle> onJobCompleted;

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

        void done(JobType* job, JobHandle handle) {
            {
                std::lock_guard<std::mutex> lock(queueAccessMutex);
                reusableIndices.push(static_cast<size_t>(handle));
                onJobCompleted(job, handle);
                delete job;
            }
        }

        WorkerType* createWorker(
            const WorkerArchetypeConfiguration<WorkerType>& configuration,
            size_t i
        ) {
            return configuration.creator(
                i,
                [&, this](JobType** pJob, JobHandle* pId) {
                    return tryRetrieveJob(pJob, pId);
                },
                [&, this](JobType* job, JobHandle id) {
                    done(job, id);
                }
            );
        }

        void ensureNumWorkersAwake(size_t num) {
            size_t numWorkersNeededToAwake = num;
            for (const auto& worker : workers) {
                if (worker->isAwake()) {
                    numWorkersNeededToAwake--;
                }
                if (numWorkersNeededToAwake == 0) {
                    return;
                }
            }
            while (numWorkersNeededToAwake > 0) {
                std::size_t lastIndex = workers.size();
                for (std::size_t i = 0 ; i < lastIndex ; ++i) {
                    if (i == lastIndex - 1) {
                        return;
                    }
                    const auto& worker = workers[i];
                    if (!worker->isAwake()) {
                        worker->awake();
                        numWorkersNeededToAwake--;
                    }
                }
            }
        }

        void unsafeDispatch(JobHandle handle) {
            ready.push(handle);
            ensureNumWorkersAwake(ready.size());
        }

    public:

        WorkerPool(JobGraph* graph) : queueAccessMutex() {

        }

        ~WorkerPool() {
            for (WorkerType* worker : workers) {
                worker->stop();
                delete worker;
            }
        }

        JobHandle enqueue(JobType* job, JobHandle graphHandle, bool dispatch) {
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
                    unsafeDispatch(handle);
                }
            }
        }


        void allocateWorkers(WorkerArchetypeConfiguration<WorkerType> configuration) {
            auto numWorkers = configuration.numWorkers;
            workers.reserve(numWorkers);
            for (std::size_t i = 0 ; i < numWorkers ; ++i) {
                workers.emplace_back(createWorker(configuration, i));
            }
        }

        std::vector<WorkerType*>& getWorkers() {
            return workers;
        }

        const std::vector<WorkerType*>& getWorkers() const {
            return workers;
        }

        un::Event<JobType*, JobHandle>& getOnJobCompleted() {
            return onJobCompleted;
        }

        const un::Event<JobType*, JobHandle>& getOnJobCompleted() const {
            return onJobCompleted;
        }
    };

}
#endif //UNNECESSARYENGINE_WORKER_POOL_H
