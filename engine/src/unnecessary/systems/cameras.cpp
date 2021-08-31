#include <unnecessary/systems/cameras.h>
#include <unnecessary/components/common.h>
#include <unnecessary/graphics/buffers/buffer.h>
#include <unnecessary/systems/graphics_systems.h>

namespace un {
    void CameraSystem::step(un::World& world, f32 delta, un::JobWorker* worker) {
        const auto& view = world.view<un::Camera, un::Projection>();
        for (auto[entity, camera, projection] : view.each()) {
            un::Matrices matrices{};
            auto viewMat = glm::identity<glm::mat4>();
            un::LocalToWorld* pLtw = world.try_get<un::LocalToWorld>(entity);
            if (pLtw != nullptr) {
                viewMat = pLtw->value;
            }

            matrices.vp = projection.value * viewMat;
            camera.cameraDescriptorBuffer.push(
                renderer->getVirtualDevice(),
                &matrices
            );

        }

    }

    CameraSystem::CameraSystem(Renderer* renderer) : renderer(renderer) {

    }

    void CameraSystem::describe(SystemDescriptor& descriptor) {
        preparation = descriptor.dependsOn<un::PrepareFrameGraphSystem>();
    }
}