#include <unnecessary/systems/cameras.h>
#include <unnecessary/components/common.h>
#include <unnecessary/graphics/buffers/buffer.h>

namespace un {
    void CameraSystem::step(un::World& world, f32 delta, un::JobWorker* worker) {
        const auto& view = world.view<un::Camera, un::Projection>();
        for (auto[entity, camera, projection] : view.each()) {
            un::Matrices matrices{};
            auto view = glm::identity<glm::mat4>();
            un::LocalToWorld* pLtw = world.try_get<un::LocalToWorld>(entity);
            if (pLtw != nullptr) {
                view = pLtw->value;
            }

            matrices.vp = projection.value * view;
            camera.cameraDescriptorBuffer.push(renderer->getVirtualDevice(), &matrices);
        }

    }

    CameraSystem::CameraSystem(Renderer* renderer) : renderer(renderer) {

    }
}