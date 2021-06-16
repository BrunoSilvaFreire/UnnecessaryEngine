#include <unnecessary/graphics/systems/drawing.h>

namespace un {
    struct DrawingData {
    public:
        DrawingData(const vk::CommandBuffer &buffer, int order) : buffer(buffer), order(order) {}

        vk::CommandBuffer buffer;
        int order;

        friend bool operator<(const DrawingData &lhs, const DrawingData &rhs) {
            return lhs.order < rhs.order;
        }

        friend bool operator>(const DrawingData &lhs, const DrawingData &rhs) {
            return rhs < lhs;
        }

        friend bool operator<=(const DrawingData &lhs, const DrawingData &rhs) {
            return !(rhs < lhs);
        }

        friend bool operator>=(const DrawingData &lhs, const DrawingData &rhs) {
            return !(lhs < rhs);
        }
    };

    void DrawingSystem::step(World &world, f32 delta, un::JobWorker *worker) {
        vk::ClearColorValue clearColorValue;
        std::array<float, 4> colorArr{};
        colorArr[0] = clearColor.r;
        colorArr[1] = clearColor.g;
        colorArr[2] = clearColor.b;
        colorArr[3] = clearColor.a;
        clearColorValue.setFloat32(colorArr);
        vk::ClearValue clear(clearColorValue);
        un::CommandBuffer unBuffer(*renderer, worker->getCommandBufferPool());
        vk::CommandBuffer commandBuffer = unBuffer.getVulkanBuffer();
        commandBuffer.begin(
                vk::CommandBufferBeginInfo(
                        (vk::CommandBufferUsageFlags) vk::CommandBufferUsageFlagBits::eOneTimeSubmit
                )
        );
        u32 framebufferIndex;
        auto framebuffer = nextFramebuffer(&framebufferIndex);
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

        //buffer.executeCommands(buffers);
        commandBuffer.endRenderPass();
        commandBuffer.end();

        vk::SubmitInfo info;
        info.pCommandBuffers = &commandBuffer;
        info.commandBufferCount = 1;
        const vk::Queue &graphicsQueue = renderer->getGraphics().getVulkan();
        graphicsQueue.submit(1, &info, nullptr);
        graphicsQueue.waitIdle();
        graphicsQueue.presentKHR(
                vk::PresentInfoKHR(
                        0, nullptr,
                        1, &renderer->getSwapChain().getSwapChain(),
                        &framebufferIndex,
                        nullptr
                )
        );
    }

    DrawingSystem::DrawingSystem(Renderer &renderer) : renderer(&renderer),
                                                       currentFramebufferIndex(0) {
        un::SwapChain &chain = renderer.getSwapChain();
        vk::AttachmentDescription colorAttachment(
                (vk::AttachmentDescriptionFlags) 0,
                chain.getFormat()
        );
        colorAttachment.setFinalLayout(vk::ImageLayout::eGeneral);
        colorAttachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
        colorAttachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;

        std::vector<vk::AttachmentReference> colorAttachments({
                                                                      vk::AttachmentReference(
                                                                              0,
                                                                              vk::ImageLayout::eColorAttachmentOptimal
                                                                      )
                                                              });
        std::vector<vk::AttachmentReference> inputAttachments({

                                                              });
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

        std::array<vk::AttachmentDescription, 1> attachmentDescriptions = {
                colorAttachment
        };
        const vk::Device &device = renderer.getVirtualDevice();

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
}