#include <unnecessary/systems/cameras.h>
#include <unnecessary/components/common.h>
#include <unnecessary/graphics/buffer.h>

namespace un {
    void CameraSystem::step(un::World& world, f32 delta, un::JobWorker* worker) {
        auto& registry = world.getRegistry();
        for (entt::entity entity : world.view<un::Camera, un::Projection>()) {
            un::Camera& camera = registry.get<un::Camera>(entity);
            un::Matrices matrices{};
            const glm::mat4& projection = registry.get<un::Projection>(entity).value;
            auto view = glm::identity<glm::mat4>();
            un::LocalToWorld* pLtw = registry.try_get<un::LocalToWorld>(entity);
            if (pLtw != nullptr) {
                view = pLtw->value;
            }
            matrices.vp = projection * view;
            camera.cameraDescriptorBuffer.push(renderer->getVirtualDevice(), &matrices);
        }

    }

    CameraSystem::CameraSystem(Renderer* renderer) : renderer(renderer) {

    }
}