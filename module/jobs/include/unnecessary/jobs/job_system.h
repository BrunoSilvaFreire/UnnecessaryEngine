#ifndef UNNECESSARYENGINE_JOBSYSTEM_H
#define UNNECESSARYENGINE_JOBSYSTEM_H

#include <queue>
#include <unnecessary/def.h>
#include <unnecessary/logging.h>
#include <unnecessary/graphs/dependency_graph.h>
#include <unnecessary/misc/types.h>
#include <unnecessary/jobs/job.h>
#include <utility>
#include <chrono>
#include <thread>
#include <semaphore>
#include <mutex>
#include <condition_variable>
#include <functional>
#include "unnecessary/misc/templates.h"

#define READ_ONLY()
#define READ_WRITE()
#define WRITE_ONLY()

namespace un {

    template<typename J>
    class JobGraph : public un::DependencyGraph<J*> {};

    template<typename T>
    class WorkerPool {
    private:
        std::vector<T> workers;
    public:
        void allocateWorkers() {
            u32 numWorkers;
        }
    };

    template<typename ...WorkerArchetypes>
    class JobSystem {
        friend class JobWorker;

    private:
        std::tuple<WorkerPool<WorkerArchetypes>...> workers;
        std::queue<JobHandle> awaitingExecution;
        std::mutex queueUsage, graphUsage;


        void awakeSomeoneUp();

        void notifyCompletion(JobHandle id);

        template<typename T, typename Seq>
        struct expander;

        template<typename T, std::size_t... Is>
        struct expander<T, std::index_sequence<Is...>> {
            template<typename E, std::size_t>
            using elem = E;

            using type = std::tuple<elem<T, Is>...>;
        };

    public:
        typedef typename expander<
            u32,
            std::make_index_sequence<sizeof...(WorkerArchetypes)>
        >::type NumWorkersTuple;

        JobSystem(
            NumWorkersTuple numWorkersPerArchetype
        ) : queueUsage(),
            graphUsage() {
            for_types_indexed<WorkerArchetypes...>(
                [&](auto t, auto index) {
                    using WorkerType = typename decltype(t)::type;
                    un::WorkerPool<WorkerType>& archWorkers = workers[index];
                    size_t numWorkers = std::get<index>(numWorkersPerArchetype);

                    if (numWorkers == 0) {
                        numWorkers = 4;
                    }

                    archWorkers.reserve(numWorkers);
                    for (size_t i = 0 ; i < numWorkers ; ++i) {
                        archWorkers.emplace_back(
                            this,
                            i,
                            false
                        );
                    }
                }
            );

        };

        ~JobSystem();

        void start();

        void addDependency(JobHandle from, JobHandle to);

        void markForExecution(JobHandle job);

        int getNumWorkers();

        template<typename T>
        void enqueue(T* job) {
            auto pool = std::get<un::index_of_type<T, WorkerArchetypes...>>(workers);
        }

        friend class World;
    };

    typedef un::JobSystem<un::JobWorker> SimpleJobSystem;
}
#endif
