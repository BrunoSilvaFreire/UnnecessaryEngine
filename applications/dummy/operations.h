//
// Created by brunorbsf on 05/10/2021.
//

#ifndef UNNECESSARYENGINE_OPERATIONS_H
#define UNNECESSARYENGINE_OPERATIONS_H

#include <GLFW/glfw3.h>
#include <unnecessary/systems/system.h>
#include <unnecessary/jobs/parallel_for_job.h>
#include <unnecessary/components/common.h>
#include <unnecessary/systems/transform.h>
#include <unnecessary/components/dummy.h>

namespace un {
    struct Randomizer {
        float speed;
        float time;
        glm::vec3 rotationAxis;
    };

    typedef std::vector<entt::entity> RandomizerArchetype;

    class RandomizerSystem : public un::ExplicitSystem {
    private:
        std::vector<entt::entity> view;
    public:
        void describe(un::SystemDescriptor& descriptor) override;

        void
        scheduleJobs(un::World& world, float deltaTime, un::JobChain& chain) override;
    };

    class ApplyRandomizationJob : public un::ParallelForJob {
    private:
        un::World* world;
        RandomizerArchetype* entities;
        float deltaTime = 1.0F / 60.0F;

        entt::entity findEntity(size_t index);

    public:
        explicit ApplyRandomizationJob(
            RandomizerArchetype* view,
            World* world
        );

        inline void operator()(size_t index, un::JobWorker* worker) override;

    };


}
#endif