#include <unnecessary/rendering/render_pass.h>

namespace un {

    un::RenderPass::RenderPass(
        const std::string& name,
        const vk::Flags<vk::PipelineStageFlagBits>& stageFlags
    ) : name(name), stageFlags(stageFlags) { }

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

    const std::optional<vk::AttachmentReference>& RenderPass::getDepthAttachment() const {
        return depthAttachment;
    }

    std::ostream& operator<<(std::ostream& os, const RenderPass& pass) {
        os << "name: " << pass.name << " stageFlags: " << vk::to_string(pass.stageFlags);
        return os;
    }

    std::ostream& operator<<(std::ostream& os, const RenderPass* pass) {
        return os << *pass;
    }
}