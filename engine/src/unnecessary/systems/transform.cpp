#include <unnecessary/systems/transform.h>

namespace un {
    void TransformSystem::step(World &world, f32 delta) {
        auto &registry = world.getRegistry();
        for (entt::entity entity : registry.view<LocalToWorld>()) {
            LocalToWorld &ltw = registry.get<LocalToWorld>(entity);
            auto matrix(glm::identity<glm::mat4>());
            if (auto translation = registry.try_get<Translation>(entity); translation) {
                matrix *= glm::translate(glm::identity<glm::mat4>(), translation->value);
            }
            if (auto rotation = registry.try_get<Rotation>(entity); rotation) {
                matrix *= glm::toMat4(rotation->value);
            }
            if (auto scale = registry.try_get<Scale>(entity); scale) {
                matrix *= glm::scale(matrix, scale->value);
            }
            ltw.value = matrix;
        }
    }
}