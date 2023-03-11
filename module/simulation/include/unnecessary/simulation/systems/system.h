#ifndef UNNECESSARYENGINE_SYSTEM_H
#define UNNECESSARYENGINE_SYSTEM_H

#include <unnecessary/jobs/worker_chain.h>
#include <unnecessary/simulation/jobs/simulation_worker.h>
#include <unnecessary/simulation/world.h>

namespace un {
    class system_descriptor {

    };

    typedef un::worker_chain<un::simulation_worker> simulation_chain;

    class system {
    public:
        virtual void schedule_jobs(
            un::world& world,
            float deltaTime,
            un::simulation_chain& chain
        ) = 0;

        virtual void describe(un::system_descriptor& descriptor) {
        }
    };
}
#endif
