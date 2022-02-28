#ifndef UNNECESSARYENGINE_JOBSYSTEM_H
#define UNNECESSARYENGINE_JOBSYSTEM_H

#include <queue>
#include <utility>
#include <chrono>
#include <thread>
#include <semaphore>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <unnecessary/def.h>
#include <unnecessary/logging.h>
#include <unnecessary/graphs/dependency_graph.h>
#include <unnecessary/jobs/job.h>
#include <unnecessary/misc/types.h>
#include <unnecessary/misc/templates.h>
#include <unnecessary/jobs/workers/worker_pool.h>

#define READ_ONLY()
#define READ_WRITE()
#define WRITE_ONLY()

namespace un {

    template<typename ...WorkerArchetypes>
    class JobSystem {
    public:
        typedef std::tuple<
            WorkerArchetypeConfiguration<WorkerArchetypes>...
        > WorkerAllocationConfig;

        typedef typename un::repeat_tuple<
            std::set<JobHandle>,
            sizeof...(WorkerArchetypes)
        >::type JobHandleSet;
        typedef std::index_sequence_for<WorkerArchetypes...> ArchetypesIndices;
        typedef std::tuple<WorkerPool<WorkerArchetypes>* ...> ArchetypesTuple;

        template<typename Archetype>
        constexpr static auto index_of_archetype() {
            return un::index_of_type<Archetype, WorkerArchetypes...>();
        }

    private:
        ArchetypesTuple workerPools;
        un::JobGraph graph;
        std::mutex graphAccessMutex;
        size_t totalNumberOfWorkers = 0;

        template<typename WorkerType, std::size_t WorkerIndex>
        void done(typename WorkerType::JobType* job, JobHandle handle) {
            {
                std::lock_guard<std::mutex> lock(graphAccessMutex);

                for (auto otherIndex : graph.dependantsOn(handle)) {
                    bool ready = true;
                    for (auto otherDependency : graph.dependenciesOf(otherIndex)) {
                        if (otherDependency != handle) {
                            // There is another dependency we need to wait for
                            ready = false;
                            break;
                        }
                    }
                    if (ready) {
                        // TODO: Ewww.
                        for_types_indexed<WorkerArchetypes...>(
                            [&]<typename OtherWorkerType, std::size_t OtherWorkerIndex>() {
                                const auto& other = graph[otherIndex];
                                if (other->archetypeIndex == OtherWorkerIndex) {
                                    auto& archWorkers = std::get<OtherWorkerIndex>(
                                        workerPools
                                    );
                                    archWorkers->unsafeDispatch(other->poolLocalIndex);
                                }
                            }
                        );
                    }
                }
                graph.remove(handle);
            }
        }

    public:


        explicit JobSystem(
            WorkerAllocationConfig numWorkersPerArchetype
        ) : graph(),
            workerPools(new WorkerPool<WorkerArchetypes>(&graph)...) {

            // Instantiate workerPools
            for_types_indexed<WorkerArchetypes...>(
                [&]<typename WorkerType, std::size_t WorkerIndex>() {
                    auto& archWorkers = std::get<WorkerIndex>(workerPools);
                    auto& workerConfig = std::get<WorkerIndex>(numWorkersPerArchetype);
                    LOG(INFO) << "Allocating " << GREEN(workerConfig.getNumWorkers())
                              << " workers of archetype "
                              << PURPLE(un::type_name_of<WorkerType>());
                    totalNumberOfWorkers += workerConfig.getNumWorkers();
                    using JobType = typename WorkerType::JobType;
                    archWorkers->allocateWorkers(workerConfig);
                    archWorkers->getOnJobCompleted() += [&](
                        JobType* job,
                        un::JobHandle graphHandle
                    ) {
                        done<WorkerType, WorkerIndex>(job, graphHandle);
                    };
                }
            );
        };

        ~JobSystem() {
            for_types_indexed<WorkerArchetypes...>(
                [&]<typename WorkerType, std::size_t WorkerIndex>() {
                    delete std::get<WorkerIndex>(workerPools);
                }
            );
        };

        void addDependency(JobHandle to, JobHandle from) {
#ifdef DEBUG
            if (from == to) {
                throw std::runtime_error("A job cannot depend on itself!");
            }
#endif
            {
                std::lock_guard<std::mutex> guard(graphAccessMutex);
                graph.addDependency(from, to);
            }
        }

        template<typename T>
        un::WorkerPool<T>* getWorkerPool() {
            constexpr std::size_t Index = index_of_archetype<T>();
            return std::get<Index>(workerPools);
        }

        template<typename T>
        JobHandle enqueue(T* job, bool dispatch) {
            using JobWorkerType = typename T::WorkerType;
            constexpr std::size_t ArchetypeIndex = un::index_of_type<JobWorkerType, WorkerArchetypes...>();
            un::WorkerPool<JobWorkerType>* pool = std::get<ArchetypeIndex>(workerPools);
            un::JobHandle graphHandle = graph.add(
                un::JobNode{
                    .archetypeIndex = ArchetypeIndex
                }
            );
            un::JobNode* node = graph[graphHandle];
            node->poolLocalIndex = pool->enqueue(job, graphHandle, dispatch);
            return graphHandle;
        }


        template<typename T, std::size_t ArchetypeIndex>
        void dispatch(JobHandle handle) {
            using JobWorkerType = typename T::WorkerType;
            un::WorkerPool<JobWorkerType>* pool = std::get<ArchetypeIndex>(workerPools);
            pool->dispatch(handle);
        }


        template<typename T>
        void dispatch(JobHandle handle) {
            using JobWorkerType = typename T::WorkerType;
            constexpr std::size_t ArchetypeIndex = un::index_of_type<JobWorkerType, WorkerArchetypes...>();
            dispatch<T, ArchetypeIndex>(handle);
        }

        template<typename WorkerType>
        void dispatch(std::set<JobHandle> handles) {
            constexpr auto ArchetypeIndex = index_of_archetype<WorkerType>();
            un::WorkerPool<WorkerType>* pool = std::get<ArchetypeIndex>(workerPools);
            pool->dispatch(handles);
        }

        void dispatch(JobHandleSet handles) {
            for_types_indexed<WorkerArchetypes...>(
                [&]<typename WorkerType, std::size_t WorkerIndex>() {
                    auto& pool = std::get<WorkerIndex>(workerPools);
                    pool->dispatch(std::get<WorkerIndex>(handles));
                }
            );
        }

        /**
         * Immediately stops this job system.
         * Any jobs that was previosly scheduled will **NOT** be executed.
         * @param block Should we block while the job system isn't stopped?
         */
        void stop(bool block) {

            if (block) {

                std::atomic<size_t> remaining = totalNumberOfWorkers;
                std::condition_variable allExited;
                std::mutex mutex;
                LOG(INFO) << "Stopping " << totalNumberOfWorkers << " total workers.";
                for_types_indexed<WorkerArchetypes...>(
                    [&]<typename WorkerType, std::size_t WorkerIndex>() {
                        auto& pool = std::get<WorkerIndex>(workerPools);
                        for (WorkerType* worker : pool->getWorkers()) {
                            worker->getOnFinished() += [&]() {
                                remaining--;
                                allExited.notify_one();
                            };
                            worker->stop();
                        }
                    }
                );
                std::unique_lock<std::mutex> lock(mutex);
                allExited.wait(
                    lock,
                    [&]() {
                        return remaining == 0;
                    }
                );

            } else {
                for_types_indexed<WorkerArchetypes...>(
                    [&]<typename WorkerType, std::size_t WorkerIndex>() {
                        auto& pool = std::get<WorkerIndex>(workerPools);
                        for (WorkerType* worker : pool->getWorkers()) {
                            worker->stop();
                        }
                    }
                );
            }
        }

        void complete() {
            for_types_indexed<WorkerArchetypes...>(
                [&]<typename WorkerType, std::size_t WorkerIndex>() {
                    auto& pool = std::get<WorkerIndex>(workerPools);
                    pool->complete();
                }
            );
            std::atomic<size_t> remaining = totalNumberOfWorkers;
            std::condition_variable allExited;
            std::mutex mutex;
            LOG(INFO) << "Stopping " << totalNumberOfWorkers << " total workers.";
            for_types_indexed<WorkerArchetypes...>(
                [&]<typename WorkerType, std::size_t WorkerIndex>() {
                    auto& pool = std::get<WorkerIndex>(workerPools);
                    for (WorkerType* worker : pool->getWorkers()) {
                        worker->getOnFinished() += [&]() {
                            remaining--;
                            allExited.notify_one();
                        };
                    }
                }
            );
            std::unique_lock<std::mutex> lock(mutex);
            allExited.wait(
                lock,
                [&]() {
                    return remaining == 0;
                }
            );
        }

        template<typename TWorker>
        friend
        class WorkerChain;

        template<typename TJobSystem>
        friend
        class JobChain;
    };

    class SimpleJobSystem : public un::JobSystem<un::JobWorker> {
    public:
        SimpleJobSystem(
            std::size_t numWorkers,
            bool autoStart
        ) : un::JobSystem<un::JobWorker>(
            un::WorkerArchetypeConfiguration<un::JobWorker>(
                numWorkers,
                [=](
                    std::size_t index,
                    un::JobProvider<un::SimpleJob> provider,
                    un::JobNotifier<un::SimpleJob> notifier
                ) {
                    return new un::JobWorker(index, autoStart, provider, notifier);
                }
            )
        ) {

        }

    };
}
#endif
