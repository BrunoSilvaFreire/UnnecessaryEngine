#ifndef UNNECESSARYENGINE_SYSTEM_H
#define UNNECESSARYENGINE_SYSTEM_H

#include <unnecessary/jobs/worker_chain.h>
#include <unnecessary/simulation/jobs/simulation_worker.h>
#include <unnecessary/simulation/world.h>

namespace un {
    class SystemDescriptor {

    };

    typedef un::WorkerChain<un::SimulationWorker> SimulationChain;

    class System {
    public:
        virtual void scheduleJobs(
            un::World& world,
            float deltaTime,
            un::SimulationChain& chain
        ) = 0;

        virtual void describe(un::SystemDescriptor& descriptor) {}
    };
}
#endif
