#include <unnecessary/graphics/systems/drawing.h>
#include <unnecessary/systems/world.h>
#include <unnecessary/components/common.h>
#include <unnecessary/graphics/buffers/command_buffer.h>
#include <unnecessary/graphics/descriptors/descriptor_allocator.h>
#include <unnecessary/graphics/descriptors/descriptor_writer.h>
#include <unnecessary/systems/cameras.h>


namespace un {

    DrawingSystem::DrawingSystem(
        Renderer* renderer
    ) : renderer(renderer) {
        un::SwapChain& chain = renderer->getSwapChain();
        const vk::Device& device = renderer->getVirtualDevice();

        un::DescriptorSetLayout perCameraDescriptorLayout,
            perObjectDescriptorLayout;

        perCameraDescriptorLayout.push<un::Matrices>(
            "matrices",
            vk::DescriptorType::eUniformBuffer
        );
        perObjectDescriptorLayout.push<un::ObjectLightingData>(
            "objectLighting",
            vk::DescriptorType::eUniformBuffer
        );

        cameraSetLayout = new un::DescriptorAllocator(
            std::move(perCameraDescriptorLayout),
            device,
            4,
            vk::ShaderStageFlagBits::eAllGraphics
        );
        objectSetLayout = new un::DescriptorAllocator(
            std::move(perObjectDescriptorLayout),
            device,
            64,
            vk::ShaderStageFlagBits::eAllGraphics
        );
    }

    void DrawingSystem::step(World& world, f32 delta, un::JobWorker* worker) {
        const vk::Device& device = renderer->getVirtualDevice();
        const vk::SwapchainKHR& chain = renderer->getSwapChain().getSwapChain();


        auto& registry = world.getRegistry();
        un::Size2D size = renderer->getSwapChain().getResolution();
        const vk::Rect2D& renderArea = vk::Rect2D(
            vk::Offset2D(0, 0),
            vk::Extent2D(size.x, size.y)
        );
        auto commandBuffer = renderingPipeline->getDrawOpaquePass()->record();
        commandBuffer.setViewport(
            0,
            {
                vk::Viewport(
                    0, 0,
                    size.x, size.y,
                    0, 1
                )
            }
        );

        commandBuffer.setScissor(
            0,
            {
                renderArea
            }
        );

        for (entt::entity cameraEntity : world.view<un::Camera, un::Projection>()) {
            const un::Projection& proj = registry.get<un::Projection>(cameraEntity);
            const un::Camera& camera = registry.get<un::Camera>(cameraEntity);

            for (entt::entity entity : world.view<un::LocalToWorld, un::RenderMesh, un::ObjectLights>()) {
                const un::LocalToWorld& ltw = registry.get<un::LocalToWorld>(entity);
                un::RenderMesh& mesh = registry.get<un::RenderMesh>(entity);
                const un::ObjectLights& lights = registry.get<un::ObjectLights>(entity);
                un::Pipeline* pPipeline = mesh.material->getShader();
                commandBuffer.bindPipeline(
                    vk::PipelineBindPoint::eGraphics,
                    pPipeline->getPipeline()
                );
                const vk::PipelineLayout& layout = pPipeline->getPipelineLayout();
                commandBuffer.bindDescriptorSets(
                    vk::PipelineBindPoint::eGraphics,
                    layout,
                    0,
                    {
                        lightingSystem->getLightsDescriptorSet(),
                        camera.cameraDescriptorSet,
                        lights.descriptorSet
                    }, {

                    }
                );
                un::PerObjectData objectData{};
                objectData.modelMatrix = ltw.value;
                commandBuffer.pushConstants<un::PerObjectData>(
                    layout,
                    vk::ShaderStageFlagBits::eVertex,
                    0, {
                        objectData
                    }
                );
                auto& meshInfo = mesh.meshInfo;
                commandBuffer.bindVertexBuffers(
                    0,
                    {
                        meshInfo->getVertex().getVulkanBuffer()
                    }, {
                        0
                    }
                );
                commandBuffer.bindIndexBuffer(
                    meshInfo->getIndex(),
                    0,
                    vk::IndexType::eUint16
                );
                commandBuffer.drawIndexed(
                    meshInfo->getIndexCount(),
                    1,
                    0,
                    0,
                    0
                );
            }
        }
    }


    void DrawingSystem::describe(SystemDescriptor& descriptor) {
        descriptor.runsOnStage(un::kRecordFrame);
        lightingSystem = descriptor.dependsOn<un::LightingSystem>();
        frameGraphSystem = descriptor.dependsOn<un::PrepareFrameGraphSystem>();
        renderingPipeline = descriptor.usesPipeline<un::PhongRenderingPipeline>(renderer);
    }


    const un::DescriptorAllocator*
    DrawingSystem::getCameraDescriptorSetAllocator() const {
        return cameraSetLayout;
    }

    const un::DescriptorAllocator* DrawingSystem::getObjectDescriptorAllocator() const {
        return objectSetLayout;
    }


    un::DescriptorAllocator* DrawingSystem::getCameraDescriptorSetAllocator() {
        return cameraSetLayout;
    }

    un::DescriptorAllocator* DrawingSystem::getObjectDescriptorAllocator() {
        return objectSetLayout;
    }
}