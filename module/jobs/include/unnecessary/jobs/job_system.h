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

    template<typename J>
    class JobGraph : public un::DependencyGraph<J*> {};


    template<typename ...WorkerArchetypes>
    class JobSystem {
        friend class JobWorker;

    private:
        std::tuple<WorkerPool<WorkerArchetypes>...> workers;
        std::queue<JobHandle> awaitingExecution;
        std::mutex queueUsage, graphUsage;


        void awakeSomeoneUp();

        void notifyCompletion(JobHandle id);

    public:
        typedef typename repeat_tuple<
            WorkerArchetypeConfiguration<WorkerArchetypes...>,
            sizeof...(WorkerArchetypes)
        >::type WorkerAllocationConfig;

        explicit JobSystem(
            WorkerAllocationConfig numWorkersPerArchetype
        ) : queueUsage(),
            graphUsage() {

            // Instantiate workers
            for_types_indexed<WorkerArchetypes...>(
                [&]<typename WorkerType, std::size_t WorkerIndex>() {
                    auto& archWorkers = std::get<WorkerIndex>(workers);
                    auto& workerConfig = std::get<WorkerIndex>(numWorkersPerArchetype);
                    LOG(INFO) << "Allocating " << GREEN(workerConfig.getNumWorkers())
                              << " workers of archetype "
                              << PURPLE(un::type_name_of<WorkerType>());

                    archWorkers.allocateWorkers(
                        workerConfig,
                        [&](typename WorkerType::JobType** jobPtr, JobHandle* id) {
                            return false;
                        },

                        [](typename WorkerType::JobType* jobPtr, JobHandle id) {

                        }
                    );
                }
            );
        };

        ~JobSystem() = default;

        void start();

        void addDependency(JobHandle from, JobHandle to);

        void markForExecution(JobHandle job);

        int getNumWorkers();

        template<typename T>
        JobHandle enqueue(T* job, bool markForExecution) {
            return 0;
        }

        friend class World;
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
