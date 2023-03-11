//
// Created by bruno on 21/12/2021.
//

#ifndef UNNECESSARYENGINE_COMPUTE_MATRICES_H
#define UNNECESSARYENGINE_COMPUTE_MATRICES_H

#include <unnecessary/jobs/parallel_for_job.h>
#include <unnecessary/simulation/jobs/simulation_worker.h>
#include <unnecessary/simulation/world.h>
#include <unnecessary/simulation/transforms/components.h>

namespace un {
    class compute_local_to_world_job : public parallel_for_job<simulation_worker> {
    private:
        world* _world;
        std::vector<entt::entity> _entities;

    public:
        compute_local_to_world_job(
            world* world,
            const std::vector<entt::entity>& entities
        );

        void operator()(size_t index, simulation_worker* worker) override;
    };
}
#endif
