#ifndef UNNECESSARYENGINE_JOB_SYSTEM_H
#define UNNECESSARYENGINE_JOB_SYSTEM_H

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
    class profiler_pool;

    template<typename... Archetypes>
    class job_system;

    template<typename... Archetypes>
    using job_system_extension = extension<job_system<Archetypes...>>;

    template<typename... Archetypes>
    class job_system
        : public extensible<job_system_extension<Archetypes...>>,
          public archetype_mixin<Archetypes> ... {
    public:
        using worker_allocation_config = std::tuple<worker_pool_configuration<Archetypes>...>;

        template<typename TValue>
        using repeated_tuple = typename repeat_tuple<TValue, sizeof...(Archetypes)>::type;

        template<template<typename> typename TValue>
        using archetype_tuple = std::tuple<TValue<Archetypes>...>;

        template<template<typename> typename TValue>
        constexpr UN_AGGRESSIVE_INLINE static auto make_archetype_tuple() {
            return std::make_tuple<TValue<Archetypes>...>();
        }

        using profiler_pool_tuple = archetype_tuple<profiler_pool>;
        using dispatch_table = job_dispatch_table<Archetypes...>;
        using archetypes_indices = std::index_sequence_for<Archetypes...>;

        template<typename Archetype>
        constexpr UN_AGGRESSIVE_INLINE static auto index_of_archetype() {
            return un::index_of_type<Archetype, Archetypes...>();
        }

        template<typename Functor>
        constexpr UN_AGGRESSIVE_INLINE static void for_each_archetype(Functor&& functor) {
            un::for_types_indexed<Archetypes...>(functor);
        }

    protected:
        job_graph _graph;
        std::mutex _graphAccessMutex;
        size_t _totalNumberOfWorkers = 0;
        event<job_handle, const job_node&> _unlockFailed;

        template<typename worker_type, std::size_t WorkerIndex>
        void done(typename worker_type::job_type* job, job_handle handle) {
            {
                std::lock_guard<std::mutex> lock(_graphAccessMutex);
                for (auto subsequent : _graph.dependants_on(handle)) {
                    bool ready = true;
                    for (auto subsequentDependency : _graph.dependencies_of(subsequent)) {
                        if (subsequentDependency != handle) {
                            // There is another dependency we need to wait for
                            ready = false;
                            break;
                        }
                    }
                    if (ready) {
                        // TODO: Ewww.
                        for_types_indexed<Archetypes...>(
                            [
                                &,
                                subsequent
                            ]<typename OtherWorkerType, std::size_t OtherWorkerIndex>() {
                                const auto& other = _graph[subsequent];
                                if (other->archetypeIndex == OtherWorkerIndex) {
                                    auto& pool = get_worker_pool<OtherWorkerType>();
                                    pool.unsafe_dispatch(other->poolLocalIndex);
                                }
                            }
                        );
                    }
                    else {
                        _unlockFailed(subsequent, *_graph.get_inner_graph().vertex(subsequent));
                    }
                    _graph.disconnect(handle, subsequent);
                }
                _graph.remove(handle);
            }
        }

        template<typename worker_type>
        std::pair<job_handle, job_node*> create(
            typename worker_type::job_type* job,
            bool dispatch
        ) {
            constexpr std::size_t archetype_index = un::index_of_type<
                worker_type,
                Archetypes...
            >();

            worker_pool<worker_type>& pool = get_worker_pool<worker_type>();
            job_handle graphHandle = _graph.add(
                job_node{
                    .archetypeIndex = archetype_index
                }
            );
            job_node* node = _graph[graphHandle];
            node->poolLocalIndex = pool.enqueue(job, graphHandle, dispatch);
            return std::make_pair(graphHandle, node);
        }

        void dispatch(dispatch_table handles) {
            for_types_indexed<Archetypes...>(
                [this, &handles]<typename worker_type, std::size_t WorkerIndex>() {
                    const std::set<job_handle>& toDispatch = handles.template get_batch<worker_type>();
                    this->archetype_mixin<worker_type>::dispatch_local(toDispatch);
                }
            );
        }

    public:
        explicit job_system(
            worker_allocation_config numWorkersPerArchetype
        ) : _graph() {
            // Instantiate workerPools
            for_types_indexed<Archetypes...>(
                [this, numWorkersPerArchetype]<typename worker_type, std::size_t WorkerIndex>() {
                    auto& pool = get_worker_pool<worker_type>();
                    auto& workerConfig = std::get<WorkerIndex>(numWorkersPerArchetype);
                    _totalNumberOfWorkers += workerConfig.get_num_workers();
                    using job_type = typename worker_type::job_type;
                    pool.allocate_workers(workerConfig);
                    pool.add_job_completed_listener(
                        [&](job_type* job, job_handle graphHandle) {
                            done<worker_type, WorkerIndex>(job, graphHandle);
                        }
                    );
                }
            );
        };

        ~job_system() {
            LOG(INFO) << "Destroying Job System";
        }

        template<typename worker_type>
        worker_pool<worker_type>& get_worker_pool() {
            return archetype_mixin<worker_type>::_pool;
        }

        template<typename worker_type>
        const worker_pool<worker_type>& get_worker_pool() const {
            return archetype_mixin<worker_type>::_pool;
        }

        void add_dependency(job_handle afterThis, job_handle runThis) {
#ifdef DEBUG
            if (afterThis == runThis) {
                throw std::runtime_error("A job cannot depend on itself!");
            }
#endif
            {
                std::lock_guard<std::mutex> guard(_graphAccessMutex);
                _graph.add_dependency(runThis, afterThis);
            }
        }

        void set_name(job_handle handle, const std::string& name) {
            const auto& other = _graph[handle];
            for_types_indexed<Archetypes...>(
                [&]<typename OtherWorkerType, std::size_t OtherWorkerIndex>() {
                    if (other->archetypeIndex == OtherWorkerIndex) {
                        auto& pool = get_worker_pool<OtherWorkerType>();
                        pool.get_job(other->poolLocalIndex)->set_name(name);
                    }
                }
            );
        }

        template<typename t_archetype>
        typename t_archetype::job_type* get_job(job_handle handle) {
            const auto& other = _graph[handle];
            auto& pool = get_worker_pool<t_archetype>();
            return pool.get_job(other->poolLocalIndex);
        }

        /**
         * Immediately stops this job system.
         * Any jobs that was previosly scheduled will **NOT** be executed.
         * @param block Should we block while the job system isn't stopped?
         */
        void stop(bool block) {
            if (block) {
                std::atomic<size_t> remaining = _totalNumberOfWorkers;
                std::condition_variable allExited;
                std::mutex mutex;
                LOG(INFO) << "Stopping " << _totalNumberOfWorkers << " total workers.";
                for_types_indexed<Archetypes...>(
                    [&]<typename worker_type, std::size_t WorkerIndex>() {
                        auto& pool = get_worker_pool<worker_type>();
                        for (worker_type* worker : pool->get_workers()) {
                            worker->get_on_finished() += [&]() {
                                --remaining;
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
            }
            else {
                for_types_indexed<Archetypes...>(
                    [&]<typename worker_type, std::size_t WorkerIndex>() {
                        auto& pool = get_worker_pool<worker_type>();
                        for (worker_type* worker : pool->get_workers()) {
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
                [&]<typename worker_type, std::size_t WorkerIndex>() {
                    auto& pool = get_worker_pool<worker_type>();
                    for (worker_type* worker : pool.get_workers()) {
                        workers.emplace(i++);
                    }
                }
            );

            std::size_t j = 0;
            fence<> fence(workers);
            for_types_indexed<Archetypes...>(
                [&]<typename worker_type, std::size_t WorkerIndex>() mutable {
                    auto& pool = get_worker_pool<worker_type>();
                    for (worker_type* worker : pool.get_workers()) {
                        worker->join(un::fence_notifier<std::size_t>(&fence, j++));
                    }
                }
            );
            fence.wait();
        }

        void join() {
            std::set<std::size_t> workers;
            std::size_t i = 0;
            for_types_indexed<Archetypes...>(
                [&]<typename worker_type, std::size_t WorkerIndex>() {
                    auto& pool = get_worker_pool<worker_type>();
                    for (worker_type* worker : pool.get_workers()) {
                        workers.emplace(i++);
                    }
                }
            );
            i = 0;
            fence<> fence(workers);
            for_types_indexed<Archetypes...>(
                [&]<typename worker_type, std::size_t WorkerIndex>() {
                    auto& pool = get_worker_pool<worker_type>();
                    for (worker_type* worker : pool.get_workers()) {
                        worker->join(i++, fence);
                    }
                }
            );
            fence.wait();
        }

        template<typename TFunc>
        void for_each_worker(TFunc block) {
            for_each_archetype(
                [&]<typename t_archetype, std::size_t TArchetypeIndex>() {
                    worker_pool<t_archetype>& pool = get_worker_pool<t_archetype>();
                    for (const auto& worker : pool.get_workers()) {
                        block.template operator()<t_archetype, TArchetypeIndex>(worker);
                    }
                }
            );
        }

        void start() {
            for_each_archetype(
                [&]<typename t_archetype, std::size_t TArchetypeIndex>() {
                    get_worker_pool<t_archetype>().start();
                }
            );
        }

        const job_graph& get_job_graph() const {
            return _graph;
        }

        event<job_handle, const job_node&>& on_unlock_failed() {
            return _unlockFailed;
        }

        const event<job_handle, const job_node&>& on_unlock_failed() const {
            return _unlockFailed;
        }

        template<typename worker_type>
        friend
        class worker_chain;

        template<typename t_job_system>
        friend
        class job_chain;
    };
}
#endif
