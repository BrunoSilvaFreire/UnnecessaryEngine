#include <unnecessary/graphics/rendering/renderpass.h>
#include <unnecessary/graphics/renderer.h>

namespace un {
    RenderPass::RenderPass(
        un::Renderer* renderer
    ) : buffer(
        *renderer,
        renderer->getGlobalPool(),
        vk::CommandBufferLevel::eSecondary
    ),
        initialized(false) {

    }

    void RenderPass::checkInitialized() const {
        if (!initialized) {
            throw std::runtime_error("GPUBuffer is not yet ready for recording.");
        }
    }

    void RenderPass::end() {
        checkInitialized();
        initialized = false;
        buffer->end();
    }

    void RenderPass::begin(const vk::CommandBufferInheritanceInfo* info) {
        initialized = true;
        buffer->reset(vk::CommandBufferResetFlagBits::eReleaseResources);
        buffer->begin(
            vk::CommandBufferBeginInfo(
                (vk::CommandBufferUsageFlags) vk::CommandBufferUsageFlagBits::eOneTimeSubmit |
                vk::CommandBufferUsageFlagBits::eRenderPassContinue,
                info
            )
        );
    }

    vk::CommandBuffer RenderPass::record() {
        checkInitialized();
        return getCommandBuffer();
    }

    vk::CommandBuffer RenderPass::getCommandBuffer() {
        return *buffer;
    }


    RenderPassDescription::RenderPassDescription(
        std::string name,
        vk::PipelineStageFlags stageFlags
    ) : name(std::move(name)),
        stageFlags(stageFlags) {
    }

    const std::vector<vk::AttachmentReference>&
    RenderPassDescription::getUsedAttachments() const {
        return usedAttachments;
    }

    const std::vector<vk::AttachmentReference>&
    RenderPassDescription::getColorAttachments() const {
        return colorAttachments;
    }

    const std::vector<vk::AttachmentReference>&
    RenderPassDescription::getResolveAttachments() const {
        return resolveAttachments;
    }

    const std::vector<vk::AttachmentReference>&
    RenderPassDescription::getDepthAttachments() const {
        return depthAttachments;
    }
}