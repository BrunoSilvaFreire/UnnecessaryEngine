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
    class ComputeLocalToWorldJob : public un::ParallelForJob<un::SimulationWorker> {
    private:
        un::World* world;
        std::vector<entt::entity> entities;
    public:

        ComputeLocalToWorldJob(
            World* world,
            const std::vector<entt::entity>& entities
        );

        void operator()(size_t index, un::SimulationWorker* worker) override;
    };
}
#endif
