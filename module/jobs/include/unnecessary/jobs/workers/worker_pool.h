//
// Created by brunorbsf on 14/12/2021.
//

#ifndef UNNECESSARYENGINE_WORKER_POOL_H
#define UNNECESSARYENGINE_WORKER_POOL_H

#include <unnecessary/jobs/workers/worker.h>

namespace un {
    template<typename T>
    class WorkerPool;

    template<typename _WorkerType>
    struct WorkerArchetypeConfiguration {
    public:
        typedef _WorkerType WorkerType;
        typedef typename WorkerType::JobType JobType;
        /**
         * Function that creates workers of each archetype during
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

        friend class WorkerPool<_WorkerType>;
    };

    template<typename _WorkerType>
    class WorkerPool {
    private:
        std::vector<_WorkerType*> workers;
        /**
         * Jobs that are ready to be executed by this pool.
         */
        std::queue<JobHandle> ready;
    public:
        typedef _WorkerType WorkerType;
        typedef typename _WorkerType::JobType JobType;

        void allocateWorkers(
            WorkerArchetypeConfiguration<WorkerType> configuration,
            JobProvider<JobType> provider,
            JobNotifier<JobType> notifier
        ) {
            auto numWorkers = configuration.numWorkers;
            workers.reserve(numWorkers);
            for (std::size_t i = 0 ; i < numWorkers ; ++i) {
                workers.emplace_back(
                    configuration.creator(
                        i,
                        provider,
                        notifier
                    )
                );
            }
        }
    };

}
#endif //UNNECESSARYENGINE_WORKER_POOL_H
