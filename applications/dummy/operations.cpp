#include "operations.h"

namespace un {

    ApplyRandomizationJob::ApplyRandomizationJob(
        RandomizerArchetype* view,
        World* world
    ) : entities(view),
        world(world) {

    };

    void RandomizerSystem::describe(un::SystemDescriptor& descriptor) {
        descriptor.runsOnStage(un::kLateGameplay);
    }

    void RandomizerSystem::scheduleJobs(
        un::World& world,
        float deltaTime,
        un::JobChain& chain
    ) {
        auto eView = world.view<un::Translation, un::Rotation, un::Scale, Randomizer>();
        view.clear();
        view.reserve(eView.size_hint());
        for (entt::entity e: eView) {
            view.emplace_back(e);
        }

        ApplyRandomizationJob* job = new ApplyRandomizationJob(&view, &world);
        if (!view.empty()) {
            un::ParallelForJob::schedule(job, chain, view.size());
        }
    }

    entt::entity ApplyRandomizationJob::findEntity(size_t index) {
        return entities->operator[](index);
    }

    [[gnu::always_inline]]
    void ApplyRandomizationJob::operator()(size_t index, un::job_worker* worker) {
        entt::entity entity = findEntity(index);
        un::Randomizer& randomizer = world->get<un::Randomizer>(entity);

        float time = randomizer.time += deltaTime;
        float scaleAmount = std::sin(time * randomizer.speed);
        un::Scale& scale = world->get<un::Scale>(entity);
        scale.value = glm::vec3(scaleAmount);

        un::Rotation& rotation = world->get<un::Rotation>(entity);
        rotation.value *= glm::quat(randomizer.rotationAxis * randomizer.speed);
    }
}