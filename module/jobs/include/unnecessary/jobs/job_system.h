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
        constexpr static void for_each_archetype(Functor&& functor) {
            un::for_types_indexed<Archetypes...>(functor);
        }

    protected:
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

        template<typename JobWorkerType>
        std::pair<un::JobHandle, un::JobNode*>
        create(typename JobWorkerType::JobType* job, bool dispatch) {
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
                    ArchetypeMixin<WorkerType>::dispatchLocal(toDispatch);
                }
            );
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

        template<typename TWorker>
        un::WorkerPool<TWorker>& getWorkerPool() {
            return ArchetypeMixin<TWorker>::_pool;
        }

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


        void setName(un::JobHandle handle, const std::string& name) {
            for_types_indexed<Archetypes...>(
                [&]<typename OtherWorkerType, std::size_t OtherWorkerIndex>() {
                    const auto& other = graph[handle];
                    if (other->archetypeIndex == OtherWorkerIndex) {
                        auto& pool = getWorkerPool<OtherWorkerType>();
                        pool.getJob(other->poolLocalIndex)->setName(name);
                    }
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
                    std::size_t index
                ) {
                    return new un::JobWorker(index, autoStart);
                }
            )
        ) {

        }

        SimpleJobSystem(
            bool autoStart
        ) : SimpleJobSystem(std::thread::hardware_concurrency(), autoStart) {

        }

        void start() {
            getWorkerPool<un::JobWorker>().start();
        }

    };
}
#endif
