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
#include <unnecessary/extension.h>
#include <unnecessary/graphs/dependency_graph.h>
#include <unnecessary/misc/types.h>
#include <unnecessary/misc/templates.h>
#include <unnecessary/jobs/job.h>
#include <unnecessary/jobs/job_dispatch.h>
#include <unnecessary/jobs/misc/fence.h>
#include <unnecessary/jobs/workers/worker_pool.h>
#include <unnecessary/jobs/workers/archetype_mixin.h>

namespace un {
    template<typename T>
    class ProfilerPool;

    template<typename ...Archetypes>
    class JobSystem;

    template<typename ...Archetypes>
    using JobSystemExtension = un::Extension<un::JobSystem<Archetypes...>>;

    template<typename ...Archetypes>
    class JobSystem : public Extensible<JobSystemExtension<Archetypes...>>, public ArchetypeMixin<Archetypes> ... {
    public:

        using WorkerAllocationConfig = std::tuple<WorkerPoolConfiguration<Archetypes>...>;

        template<typename TValue>
        using RepeatedTuple = typename un::repeat_tuple<TValue, sizeof...(Archetypes)>::type;

        template<template<typename> typename TValue>
        using ArchetypeTuple = std::tuple<TValue<Archetypes>...>;

        template<template<typename> typename TValue>
        constexpr UN_AGGRESSIVE_INLINE static auto make_archetype_tuple() {
            return std::make_tuple<TValue<Archetypes>...>();
        }


        typedef ArchetypeTuple<ProfilerPool> ProfilerPoolTuple;


        typedef un::JobDispatchTable<Archetypes...> DispatchTable;
        typedef std::index_sequence_for<Archetypes...> ArchetypesIndices;

        template<typename Archetype>
        constexpr UN_AGGRESSIVE_INLINE static auto index_of_archetype() {
            return un::index_of_type<Archetype, Archetypes...>();
        }

        template<typename Functor>
        constexpr UN_AGGRESSIVE_INLINE static void for_each_archetype(Functor&& functor) {
            un::for_types_indexed<Archetypes...>(functor);
        }

    protected:
        un::JobGraph graph;
        std::mutex graphAccessMutex;
        size_t totalNumberOfWorkers = 0;
        un::Event<un::JobHandle, const un::JobNode&> unlockFailed;

        template<typename WorkerType, std::size_t WorkerIndex>
        void done(typename WorkerType::JobType* job, JobHandle handle) {
            {
                std::lock_guard<std::mutex> lock(graphAccessMutex);
                for (auto subsequent : graph.dependantsOn(handle)) {
                    bool ready = true;
                    for (auto subsequentDependency : graph.dependenciesOf(subsequent)) {
                        if (subsequentDependency != handle) {
                            // There is another dependency we need to wait for
                            ready = false;
                            break;
                        }
                    }
                    if (ready) {
                        // TODO: Ewww.
                        for_types_indexed<Archetypes...>(
                            [&, subsequent]<typename OtherWorkerType, std::size_t OtherWorkerIndex>() {
                                const auto& other = graph[subsequent];
                                if (other->archetypeIndex == OtherWorkerIndex) {
                                    auto& pool = getWorkerPool<OtherWorkerType>();
                                    pool.unsafeDispatch(other->poolLocalIndex);
                                }
                            }
                        );
                    } else {
                        unlockFailed(subsequent, *graph.getInnerGraph().vertex(subsequent));
                    }
                    graph.disconnect(handle, subsequent);
                }
                graph.remove(handle);
            }
        }

        template<typename JobWorkerType>
        std::pair<un::JobHandle, un::JobNode*> create(
            typename JobWorkerType::JobType* job,
            bool dispatch
        ) {
            constexpr std::size_t ArchetypeIndex = un::index_of_type<JobWorkerType, Archetypes...>();
            un::WorkerPool<JobWorkerType>& pool = getWorkerPool<JobWorkerType>();
            un::JobHandle graphHandle = graph.add(
                un::JobNode{
                    .archetypeIndex = ArchetypeIndex
                }
            );
            un::JobNode* node = graph[graphHandle];
            node->poolLocalIndex = pool.enqueue(job, graphHandle, dispatch);
            return std::make_pair(graphHandle, node);
        }

        void dispatch(DispatchTable handles) {
            for_types_indexed<Archetypes...>(
                [this, &handles]<typename WorkerType, std::size_t WorkerIndex>() {
                    const std::set<JobHandle>& toDispatch = handles.template getBatch<WorkerType>();
                    this->ArchetypeMixin<WorkerType>::dispatchLocal(toDispatch);
                }
            );
        }


    public:


        explicit JobSystem(
            WorkerAllocationConfig numWorkersPerArchetype
        ) : graph() {
            // Instantiate workerPools
            for_types_indexed<Archetypes...>(
                [this, numWorkersPerArchetype]<typename WorkerType, std::size_t WorkerIndex>() {
                    auto& pool = getWorkerPool<WorkerType>();
                    auto& workerConfig = std::get<WorkerIndex>(numWorkersPerArchetype);
                    totalNumberOfWorkers += workerConfig.getNumWorkers();
                    using JobType = typename WorkerType::JobType;
                    pool.allocateWorkers(workerConfig);
                    pool.addJobCompletedListener(
                        [&](JobType* job, un::JobHandle graphHandle) {
                            done<WorkerType, WorkerIndex>(job, graphHandle);
                        }
                    );
                }
            );
        };

        template<typename TWorker>
        un::WorkerPool<TWorker>& getWorkerPool() {
            return ArchetypeMixin<TWorker>::_pool;
        }

        template<typename TWorker>
        const un::WorkerPool<TWorker>& getWorkerPool() const {
            return ArchetypeMixin<TWorker>::_pool;
        }

        void addDependency(JobHandle afterThis, JobHandle runThis) {
#ifdef DEBUG
            if (afterThis == runThis) {
                throw std::runtime_error("A job cannot depend on itself!");
            }
#endif
            {
                std::lock_guard<std::mutex> guard(graphAccessMutex);
                graph.addDependency(runThis, afterThis);
            }
        }


        void setName(un::JobHandle handle, const std::string& name) {
            const auto& other = graph[handle];
            for_types_indexed<Archetypes...>(
                [&]<typename OtherWorkerType, std::size_t OtherWorkerIndex>() {
                    if (other->archetypeIndex == OtherWorkerIndex) {
                        auto& pool = getWorkerPool<OtherWorkerType>();
                        pool.getJob(other->poolLocalIndex)->setName(name);
                    }
                }
            );
        }

        template<typename TArchetype>
        typename TArchetype::JobType* getJob(un::JobHandle handle) {
            const auto& other = graph[handle];
            auto& pool = getWorkerPool<TArchetype>();
            return pool.getJob(other->poolLocalIndex);
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
            std::set<std::size_t> workers;

            std::size_t i = 0;
            for_types_indexed<Archetypes...>(
                [&]<typename WorkerType, std::size_t WorkerIndex>() {
                    auto& pool = getWorkerPool<WorkerType>();
                    for (WorkerType* worker : pool.getWorkers()) {
                        workers.emplace(i++);
                    }
                }
            );

            std::size_t j = 0;
            un::Fence<> completionFence(workers);
            for_types_indexed<Archetypes...>(
                [&]<typename WorkerType, std::size_t WorkerIndex>() {
                    auto& pool = getWorkerPool<WorkerType>();
                    for (WorkerType* worker : pool.getWorkers()) {
                        worker->join(j++, completionFence);
                    }
                    pool.complete();
                }
            );

            completionFence.wait();
        }

        void join() {
            std::set<std::size_t> workers;
            std::size_t i = 0;
            for_types_indexed<Archetypes...>(
                [&]<typename WorkerType, std::size_t WorkerIndex>() {
                    auto& pool = getWorkerPool<WorkerType>();
                    for (WorkerType* worker : pool.getWorkers()) {
                        workers.emplace(i++);
                    }
                }
            );
            i = 0;
            un::Fence<> fence(workers);
            for_types_indexed<Archetypes...>(
                [&]<typename WorkerType, std::size_t WorkerIndex>() {
                    auto& pool = getWorkerPool<WorkerType>();
                    for (WorkerType* worker : pool.getWorkers()) {
                        worker->join(i++, fence);
                    }
                }
            );
            fence.wait();
        }

        template<typename TFunc>
        void for_each_worker(TFunc block) {
            for_each_archetype(
                [&]<typename TArchetype, std::size_t TArchetypeIndex>() {
                    un::WorkerPool<TArchetype>& pool = getWorkerPool<TArchetype>();
                    for (const auto& worker : pool.getWorkers()) {
                        block.template operator()<TArchetype, TArchetypeIndex>(worker);
                    }
                }
            );
        }

        void start() {
            for_each_archetype(
                [&]<typename TArchetype, std::size_t TArchetypeIndex>() {
                    getWorkerPool<TArchetype>().start();
                }
            );
        }

        const JobGraph& getJobGraph() const {
            return graph;
        }

        Event<un::JobHandle, const un::JobNode&>& onUnlockFailed() {
            return unlockFailed;
        }

        const Event<un::JobHandle, const un::JobNode&>& onUnlockFailed() const {
            return unlockFailed;
        }

        template<typename TWorker>
        friend
        class WorkerChain;

        template<typename TJobSystem>
        friend
        class JobChain;
    };

}
#endif
