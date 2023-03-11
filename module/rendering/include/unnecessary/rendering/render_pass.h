//
// Created by bruno on 20/12/2021.
//

#ifndef UNNECESSARYENGINE_RENDER_PASS_H
#define UNNECESSARYENGINE_RENDER_PASS_H

#include <string>
#include <unnecessary/rendering/render_graph.h>
#include <unnecessary/rendering/buffers/command_buffer.h>
#include <ostream>

namespace un {
    class render_pass {
    private:
        std::string _name;
        vk::PipelineStageFlags _stageFlags;
        std::vector<vk::AttachmentReference> _colorAttachments;
        std::vector<vk::AttachmentReference> _usedAttachments;
        std::vector<vk::AttachmentReference> _resolveAttachments;
        std::optional<vk::AttachmentReference> _depthAttachment;

    protected:
        void uses_color_attachment(std::size_t attachmentIndex, vk::ImageLayout layout) {
            _colorAttachments.emplace_back(static_cast<u32>(attachmentIndex), layout);
        }

        void uses_depth_attachment(std::size_t attachmentIndex, vk::ImageLayout layout) {
            _depthAttachment = vk::AttachmentReference(static_cast<u32>(attachmentIndex), layout);
        }

    public:
        render_pass(const std::string& name, const vk::PipelineStageFlags& stageFlags);

        virtual ~render_pass() = default;

        virtual void on_vulkan_pass_created(vk::RenderPass renderPass, renderer& renderer) {
        }

        virtual void record(
            const frame_data& data,
            command_buffer& cmdBuffer
        ) const = 0;

        const std::string& get_name() const;

        const vk::PipelineStageFlags& get_stage_flags() const;

        const std::vector<vk::AttachmentReference>& get_color_attachments() const;

        const std::vector<vk::AttachmentReference>& get_used_attachments() const;

        const std::vector<vk::AttachmentReference>& get_resolve_attachments() const;

        const std::optional<vk::AttachmentReference>& get_depth_attachment() const;

        friend std::ostream& operator<<(std::ostream& os, const render_pass& pass);

        friend std::ostream& operator<<(std::ostream& os, const render_pass* pass);
    };
}
#endif //UNNECESSARYENGINE_RENDER_PASS_H
