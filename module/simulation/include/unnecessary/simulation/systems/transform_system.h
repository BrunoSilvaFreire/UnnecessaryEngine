#ifndef UNNECESSARYENGINE_TRANSFORM_SYSTEM_H
#define UNNECESSARYENGINE_TRANSFORM_SYSTEM_H

#include "system.h"
#include "unnecessary/simulation/transforms/compute_matrices.h"

namespace un {
    class TransformSystem : public un::System {
        void scheduleJobs(
            un::World& world,
            float deltaTime,
            un::SimulationChain& chain
        ) override;
    };
}
#endif
