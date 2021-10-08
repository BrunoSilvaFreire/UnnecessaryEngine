#include "operations.h"

namespace un {

    ApplyRandomizationJob::ApplyRandomizationJob(RandomizerArchetype* view) : view(view) {

    };

    void RandomizerSystem::scheduleJobs(
        un::World& world,
        float deltaTime,
        un::JobChain& chain
    ) {
        RandomizerArchetype kek = world.view<un::Translation, un::Rotation, un::Scale, Randomizer>();
        kek.
        view = std::move(kek);
        ApplyRandomizationJob* job = new ApplyRandomizationJob(&view);
        job->schedule(chain, view.size_hint());


    }

    void ApplyRandomizationJob::operator()(size_t index, un::JobWorker* worker) {
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