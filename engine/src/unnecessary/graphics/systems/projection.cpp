
#include <utility>
#include <unnecessary/graphics/systems/projection.h>
#include <unnecessary/systems/world.h>
#include <unnecessary/graphics/buffers/command_buffer.h>

namespace un {

    void ProjectionSystem::step(World& world, f32 delta, un::JobWorker* worker) {
        entt::registry& registry = world.getRegistry();
        auto view = registry.view<un::Camera, un::Projection, un::Perspective>();
        for (entt::entity entity : view) {
            un::Camera& camera = registry.get<un::Camera>(entity);
            un::Projection& projectile = registry.get<un::Projection>(entity);
            un::Perspective& perspective = registry.get<un::Perspective>(entity);
            projectile.value = glm::perspective(
                perspective.fieldOfView,
                perspective.aspect,
                perspective.zNear,
                perspective.zFar
            );
        }
    }
}