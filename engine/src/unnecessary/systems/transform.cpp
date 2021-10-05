#include <unnecessary/systems/transform.h>

namespace un {
    void TransformSystem::step(World& world, f32 delta, un::JobWorker* worker) {
        auto& registry = world.getRegistry();
        auto view = registry.view<LocalToWorld>();
        for (entt::entity entity: view) {
            LocalToWorld& ltw = registry.get<LocalToWorld>(entity);
            auto matrix(glm::identity<glm::mat4>());


            if (auto rotation = registry.try_get<Rotation>(entity); rotation) {
                matrix *= glm::toMat4(rotation->value);
            }
            if (auto scale = registry.try_get<Scale>(entity); scale) {
                matrix = glm::scale(matrix, scale->value);
            }
            if (auto translation = registry.try_get<Translation>(entity); translation) {
                matrix *= glm::translate(glm::identity<glm::mat4>(), translation->value);
            }
            ltw.value = matrix;
        }
    }

    void TransformSystem::describe(SystemDescriptor& descriptor) {
        descriptor.runsOnStage(un::kPrepareFrame);
    }
}