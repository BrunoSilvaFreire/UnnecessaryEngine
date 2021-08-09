#include <unnecessary/graphics/systems/drawing.h>
#include <unnecessary/graphics/lighting.h>
#include <unnecessary/systems/world.h>
#include <unnecessary/components/common.h>

namespace un {

    DrawingSystem::DrawingSystem(
            Renderer &renderer
    ) : renderer(&renderer), currentFramebufferIndex(0) {
        un::SwapChain &chain = renderer.getSwapChain();
        clearColor = glm::vec4(0.1, 0.1, 0.1, 1);

        const vk::Device &device = renderer.getVirtualDevice();
        std::vector<vk::AttachmentReference> colorAttachments(
                {
                        vk::AttachmentReference(
                                0,
                                vk::ImageLayout::eColorAttachmentOptimal
                        )
                }
        );
        std::vector<vk::AttachmentReference> inputAttachments(
                {

                }
        );
        std::vector<vk::SubpassDescription> subpasses(
                {
                        vk::SubpassDescription(

                                (vk::SubpassDescriptionFlags) 0,
                                vk::PipelineBindPoint::eGraphics,
                                inputAttachments,
                                colorAttachments
                        )
                }
        );


        vk::AttachmentDescription colorAttachment(
                (vk::AttachmentDescriptionFlags) 0,
                chain.getFormat(),
                vk::SampleCountFlagBits::e1,
                vk::AttachmentLoadOp::eClear,
                vk::AttachmentStoreOp::eStore,
                vk::AttachmentLoadOp::eDontCare,
                vk::AttachmentStoreOp::eDontCare,
                vk::ImageLayout::eUndefined,
                vk::ImageLayout::ePresentSrcKHR
        );
        std::array<vk::AttachmentDescription, 1> attachmentDescriptions = {
                colorAttachment
        };
        renderPass = device.createRenderPass(
                vk::RenderPassCreateInfo(
                        (vk::RenderPassCreateFlags) 0,
                        attachmentDescriptions,
                        subpasses
                )
        );
        const un::Size2D &resolution = chain.getResolution();
        std::vector<vk::ImageView> views;
        for (const auto &item : chain.getViews()) {
            auto view = item.getVulkanView();
            framebuffers.emplace_back(
                    device.createFramebuffer(
                            vk::FramebufferCreateInfo(
                                    (vk::FramebufferCreateFlags) 0,
                                    renderPass,
                                    1, &view,
                                    resolution.x,
                                    resolution.y,
                                    1
                            )
                    )
            );
        }
        imageAvailableSemaphore = device.createSemaphore(
                vk::SemaphoreCreateInfo(
                        (vk::SemaphoreCreateFlags) 0
                )
        );
    }

    void DrawingSystem::step(World &world, f32 delta, un::JobWorker *worker) {
        vk::ClearColorValue clearColorValue;
        const vk::Device &device = renderer->getVirtualDevice();
        const vk::SwapchainKHR &chain = renderer->getSwapChain().getSwapChain();
        std::array<float, 4> colorArr{};
        colorArr[0] = clearColor.r;
        colorArr[1] = clearColor.g;
        colorArr[2] = clearColor.b;
        colorArr[3] = clearColor.a;
        clearColorValue.setFloat32(colorArr);
        vk::ClearValue clear(clearColorValue);
        un::CommandBuffer unBuffer(*renderer, worker->getGraphicsResources().getCommandPool());
        vk::CommandBuffer commandBuffer = unBuffer.getVulkanBuffer();

        auto &registry = world.getRegistry();


        /* bool resized = lightingSystem->getSceneLightingBuffer.ensureFits<un::PointLightData>(
                 *renderer,
                 pointLights.size(),
                 sizeof(int)
         );

         if (resized) {
             vk::DescriptorBufferInfo bufferInfo;
             bufferInfo.buffer = lightingBuffer.getVulkanBuffer();
             bufferInfo.range = VK_WHOLE_SIZE;
             device.updateDescriptorSets(
                     {
                             vk::WriteDescriptorSet(

                                     globalSet,
                                     1,
                                     0,
                                     1,
                                     vk::DescriptorType::eUniformBuffer,
                                     nullptr,
                                     &bufferInfo,
                                     nullptr
                             )
                     }, {

                     }
             );
         }*/
        commandBuffer.begin(
                vk::CommandBufferBeginInfo(
                        (vk::CommandBufferUsageFlags) vk::CommandBufferUsageFlagBits::eOneTimeSubmit
                )
        );

        u32 framebufferIndex;
        auto framebuffer = nextFramebuffer(&framebufferIndex);
        vkCall(
                device.acquireNextImageKHR(
                        chain,
                        std::numeric_limits<u64>::max(),
                        imageAvailableSemaphore,
                        nullptr,
                        &framebufferIndex
                )
        );
        un::Size2D size = renderer->getSwapChain().getResolution();
        const vk::Rect2D &renderArea = vk::Rect2D(
                vk::Offset2D(0, 0),
                vk::Extent2D(size.x, size.y)
        );

        commandBuffer.beginRenderPass(
                vk::RenderPassBeginInfo(
                        renderPass,
                        framebuffer,
                        renderArea,
                        1,
                        &clear
                ),
                vk::SubpassContents::eInline
        );
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
            const un::Projection &proj = registry.get<un::Projection>(cameraEntity);
            const un::Camera &camera = registry.get<un::Camera>(cameraEntity);

            for (entt::entity entity : world.view<un::LocalToWorld, un::RenderMesh, un::ObjectLights>()) {
                const un::LocalToWorld &ltw = registry.get<un::LocalToWorld>(entity);
                un::RenderMesh &mesh = registry.get<un::RenderMesh>(entity);
                const un::ObjectLights &lights = registry.get<un::ObjectLights>(entity);
                un::Pipeline *pPipeline = mesh.material->getShader();
                commandBuffer.bindPipeline(
                        vk::PipelineBindPoint::eGraphics,
                        pPipeline->getPipeline()
                );
                const vk::PipelineLayout &layout = pPipeline->getPipelineLayout();
                commandBuffer.bindDescriptorSets(
                        vk::PipelineBindPoint::eGraphics,
                        layout,
                        0,
                        {
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
                auto &meshInfo = mesh.meshInfo;
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

        commandBuffer.endRenderPass();

        commandBuffer.end();


        vk::SubmitInfo info;
        info.pCommandBuffers = &commandBuffer;
        info.commandBufferCount = 1;
        const vk::Queue &graphicsQueue = renderer->getGraphics().getVulkan();
        vkCall(graphicsQueue.submit(1, &info, nullptr));
        graphicsQueue.waitIdle();
        vkCall(graphicsQueue.presentKHR(
                vk::PresentInfoKHR(
                        1, &imageAvailableSemaphore,
                        1, &chain,
                        &framebufferIndex,
                        nullptr
                )
        ));
    }

    const vk::RenderPass &DrawingSystem::getRenderPass() const {
        return renderPass;
    }

    vk::Framebuffer DrawingSystem::nextFramebuffer(u32 *framebufferIndexResult) {
        *framebufferIndexResult = currentFramebufferIndex;
        auto buffer = framebuffers[currentFramebufferIndex++];
        currentFramebufferIndex %= framebuffers.size();
        return buffer;
    }

    void DrawingSystem::describe(SystemDescriptor &descriptor) {
        lightingSystem = descriptor.dependsOn<un::LightingSystem>();
    }
}