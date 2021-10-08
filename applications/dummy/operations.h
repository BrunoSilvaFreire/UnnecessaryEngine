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

    typedef entt::basic_view<
        entt::entity,
        entt::exclude_t<>,
        Translation,
        Rotation,
        Scale,
        Randomizer
    > RandomizerArchetype;

    class RandomizerSystem : public un::ExplicitSystem {
    private:
        RandomizerArchetype view;
    public:
        void describe(un::SystemDescriptor& descriptor) override;

        void
        scheduleJobs(un::World& world, float deltaTime, un::JobChain& chain) override;
    };

    class ApplyRandomizationJob : public un::ParallelForJob {
    private:
        RandomizerArchetype* view;
        un::World* world;
        std::vector<entt::entity> entities;
        float deltaTime;

        entt::entity findEntity(size_t index);

    public:
        explicit ApplyRandomizationJob(
            RandomizerArchetype* view
        );

        void operator()(size_t index, un::JobWorker* worker) override;

    };


    void RandomizerSystem::describe(un::SystemDescriptor& descriptor) {
        descriptor.runsOnStage(un::kLateGameplay);
    }



}
#endif