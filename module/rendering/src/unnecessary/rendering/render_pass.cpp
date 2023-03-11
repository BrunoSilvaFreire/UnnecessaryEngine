#include <unnecessary/rendering/render_pass.h>

namespace un {
    render_pass::render_pass(
        const std::string& name,
        const vk::Flags<vk::PipelineStageFlagBits>& stageFlags
    ) : _name(name), _stageFlags(stageFlags) {
    }

    const std::string& render_pass::get_name() const {
        return _name;
    }

    const vk::PipelineStageFlags& render_pass::get_stage_flags() const {
        return _stageFlags;
    }

    const std::vector<vk::AttachmentReference>& render_pass::get_color_attachments() const {
        return _colorAttachments;
    }

    const std::vector<vk::AttachmentReference>& render_pass::get_used_attachments() const {
        return _usedAttachments;
    }

    const std::vector<vk::AttachmentReference>&
    render_pass::get_resolve_attachments() const {
        return _resolveAttachments;
    }

    const std::optional<vk::AttachmentReference>& render_pass::get_depth_attachment() const {
        return _depthAttachment;
    }

    std::ostream& operator<<(std::ostream& os, const render_pass& pass) {
        os << "name: " << pass._name << " stageFlags: " << vk::to_string(pass._stageFlags);
        return os;
    }

    std::ostream& operator<<(std::ostream& os, const render_pass* pass) {
        return os << *pass;
    }
}
