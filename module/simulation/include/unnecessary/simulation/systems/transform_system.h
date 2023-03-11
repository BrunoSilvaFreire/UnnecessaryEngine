#ifndef UNNECESSARYENGINE_TRANSFORM_SYSTEM_H
#define UNNECESSARYENGINE_TRANSFORM_SYSTEM_H

#include "system.h"
#include "unnecessary/simulation/transforms/compute_matrices.h"

namespace un {
    class transform_system : public un::system {
        void schedule_jobs(
            un::world& world,
            float deltaTime,
            un::simulation_chain& chain
        ) override;
    };
}
#endif
