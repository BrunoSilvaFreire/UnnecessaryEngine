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
#include <unnecessary/misc/types.h>
#include <unnecessary/misc/templates.h>
#include <unnecessary/jobs/job.h>
#include <unnecessary/jobs/job_dispatch.h>
#include <unnecessary/jobs/workers/worker_pool.h>
#include <unnecessary/jobs/workers/archetype_mixin.h>


namespace un {
    template<typename T>
    class ProfilerPool;

    template<typename ...Archetypes>
    class JobSystem : public ArchetypeMixin<Archetypes> ... {
    public:
        typedef std::tuple<
            WorkerPoolConfiguration<Archetypes>...
        > WorkerAllocationConfig;

        typedef std::tuple<ProfilerPool<Archetypes>...> ProfilerPoolTuple;

        template<typename TValue>
        using RepeatedTuple = typename un::repeat_tuple<TValue, sizeof...(Archetypes)>::type;

        typedef un::JobDispatchTable<Archetypes...> DispatchTable;
        typedef std::index_sequence_for<Archetypes...> ArchetypesIndices;

        template<typename Archetype>
        constexpr static auto index_of_archetype() {
            return un::index_of_type<Archetype, Archetypes...>();
        }

        template<typename Functor>
        constexpr static auto for_each_archetype(Functor&& functor) {
            un::for_types_indexed<Archetypes...>(functor);
        }

    private:
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
                        for_types_indexed<Archetypes...>(
                            [&]<typename OtherWorkerType, std::size_t OtherWorkerIndex>() {
                                const auto& other = graph[otherIndex];
                                if (other->archetypeIndex == OtherWorkerIndex) {
                                    auto& pool = getWorkerPool<OtherWorkerType>();
                                    pool.unsafeDispatch(other->poolLocalIndex);
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
        ) : graph() {

            // Instantiate workerPools
            for_types_indexed<Archetypes...>(
                [&]<typename WorkerType, std::size_t WorkerIndex>() {
                    auto& pool = getWorkerPool<WorkerType>();
                    auto& workerConfig = std::get<WorkerIndex>(numWorkersPerArchetype);
//                    LOG(INFO) << "Allocating " << GREEN(workerConfig.getNumWorkers())
//                              << " workers of archetype "
//                              << PURPLE(un::type_name_of<WorkerType>());
                    totalNumberOfWorkers += workerConfig.getNumWorkers();
                    using JobType = typename WorkerType::JobType;
                    pool.allocateWorkers(workerConfig);
                    pool.getOnJobCompleted() += [&](
                        JobType* job,
                        un::JobHandle graphHandle
                    ) {
                        done<WorkerType, WorkerIndex>(job, graphHandle);
                    };
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

        template<typename TWorker>
        un::WorkerPool<TWorker>& getWorkerPool() {
            return ArchetypeMixin<TWorker>::_pool;
        }

        template<typename TJob>
        JobHandle enqueue(TJob* job, bool dispatch) {
            using JobWorkerType = typename TJob::WorkerType;
            constexpr std::size_t ArchetypeIndex = un::index_of_type<JobWorkerType, Archetypes...>();
            un::WorkerPool<JobWorkerType>& pool = getWorkerPool<JobWorkerType>();
            un::JobHandle graphHandle = graph.add(
                un::JobNode{
                    .archetypeIndex = ArchetypeIndex
                }
            );
            un::JobNode* node = graph[graphHandle];
            node->poolLocalIndex = pool.enqueue(job, graphHandle, dispatch);
            return graphHandle;
        }

/*

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
*/

/*
        template<typename WorkerType>
        void dispatch(std::set<JobHandle> handles) {
            constexpr auto ArchetypeIndex = index_of_archetype<WorkerType>();
            un::WorkerPool<WorkerType>* pool = std::get<ArchetypeIndex>(workerPools);
            pool->dispatch(handles);
        }
*/
        void dispatch(DispatchTable handles) {
            for_types_indexed<Archetypes...>(
                [&, this]<typename WorkerType, std::size_t WorkerIndex>() {
                    ArchetypeMixin<WorkerType>::dispatchLocal(
                        std::get<WorkerIndex>(handles)
                    );
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
                for_types_indexed<Archetypes...>(
                    [&]<typename WorkerType, std::size_t WorkerIndex>() {
                        auto& pool = getWorkerPool<WorkerType>();
                        for (WorkerType* worker : pool->getWorkers()) {
                            worker->getOnFinished() += [&]() {
                                remaining--;
                                allExited.notify_one();
                            };
                            worker->stop(false);
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
                for_types_indexed<Archetypes...>(
                    [&]<typename WorkerType, std::size_t WorkerIndex>() {
                        auto& pool = getWorkerPool<WorkerType>();
                        for (WorkerType* worker : pool->getWorkers()) {
                            worker->stop(false);
                        }
                    }
                );
            }
        }

        void complete() {
            std::size_t remaining = totalNumberOfWorkers;
            std::condition_variable allExited;
            std::mutex remainingLock;
            for_types_indexed<Archetypes...>(
                [&]<typename WorkerType, std::size_t WorkerIndex>() {
                    auto& pool = getWorkerPool<WorkerType>();
                    for (WorkerType* worker : pool.getWorkers()) {
                        if (worker->isRunning()) {
                            worker->getOnFinished() += [&]() {
                                std::unique_lock<std::mutex> lock(remainingLock);
                                remaining--;
                                if (remaining == 0) {
                                    allExited.notify_one();
                                }
                            };
                        } else {
                            remaining--;
                        }
                    }
                    pool.complete();
                }
            );
            if (remaining > 0) {
                std::mutex completionMutex;
                std::unique_lock<std::mutex> lock(completionMutex);
                allExited.wait(lock);
            }
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
            un::WorkerPoolConfiguration<un::JobWorker>(
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

        void start() {
            getWorkerPool<un::JobWorker>().start();
        }

    };
}
#endif
