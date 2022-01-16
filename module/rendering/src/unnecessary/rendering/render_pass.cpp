#include <unnecessary/rendering/render_pass.h>

namespace un {

    un::RenderPass::RenderPass(
        const std::basic_string<char, std::char_traits<char>, std::allocator<char>>& name,
        const vk::Flags<vk::PipelineStageFlagBits>& stageFlags
    ) : name(name), stageFlags(stageFlags) {}

    const std::string& RenderPass::getName() const {
        return name;
    }

    const vk::PipelineStageFlags& RenderPass::getStageFlags() const {
        return stageFlags;
    }

    const std::vector<vk::AttachmentReference>& RenderPass::getColorAttachments() const {
        return colorAttachments;
    }

    const std::vector<vk::AttachmentReference>& RenderPass::getUsedAttachments() const {
        return usedAttachments;
    }

    const std::vector<vk::AttachmentReference>&
    RenderPass::getResolveAttachments() const {
        return resolveAttachments;
    }

    const std::vector<vk::AttachmentReference>& RenderPass::getDepthAttachments() const {
        return depthAttachments;
    }
}