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

    void DrawingSystem::step(World &world, f32 delta) {
        vk::ClearColorValue clearColorValue;
        std::array<float, 4> colorArr{};
        colorArr[0] = clearColor.r;
        colorArr[1] = clearColor.g;
        colorArr[2] = clearColor.b;
        colorArr[3] = clearColor.a;
        clearColorValue.setFloat32(colorArr);
        vk::ClearValue clear(clearColorValue);
        vk::Framebuffer framebuffer;
        vk::CommandBuffer commandBuffer = buffer.getVulkanBuffer();
        commandBuffer.begin(
                vk::CommandBufferBeginInfo(
                        (vk::CommandBufferUsageFlags) vk::CommandBufferUsageFlagBits::eOneTimeSubmit
                )
        );
        commandBuffer.beginRenderPass(
                vk::RenderPassBeginInfo(
                        renderPass,
                        framebuffer,
                        renderArea,
                        1,
                        &clear
                ), contents
        );

        //buffer.executeCommands(buffers);
        commandBuffer.endRenderPass();
        commandBuffer.end();

        vk::SubmitInfo info;
        info.pCommandBuffers = &commandBuffer;
        info.commandBufferCount = 1;
        renderer->getGraphics().getVulkan().submit(1, &info, nullptr);
    }

    DrawingSystem::DrawingSystem(Renderer &renderer) : renderer(&renderer), buffer(renderer) {}
}