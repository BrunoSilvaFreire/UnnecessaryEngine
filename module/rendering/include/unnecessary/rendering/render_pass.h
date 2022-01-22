//
// Created by bruno on 20/12/2021.
//

#ifndef UNNECESSARYENGINE_RENDER_PASS_H
#define UNNECESSARYENGINE_RENDER_PASS_H

#include <string>
#include <unnecessary/rendering/render_graph.h>
#include <unnecessary/rendering/buffers/command_buffer.h>

namespace un {

    class RenderPass {
    private:
        std::string name;
        vk::PipelineStageFlags stageFlags;
        std::vector<vk::AttachmentReference> colorAttachments;
        std::vector<vk::AttachmentReference> usedAttachments;
        std::vector<vk::AttachmentReference> resolveAttachments;
        std::vector<vk::AttachmentReference> depthAttachments;
    protected:

        void usesAttachment(std::size_t attachmentIndex, vk::ImageLayout layout) {

        }

        void usesColorAttachment(std::size_t attachmentIndex, vk::ImageLayout layout) {

        }

        void usesDepthAttachment(std::size_t attachmentIndex, vk::ImageLayout layout) {

        }

    public:
        RenderPass(const std::string& name, const vk::PipelineStageFlags& stageFlags);

        virtual ~RenderPass() = default;

        virtual void record(
            const un::FrameData& data,
            un::CommandBuffer& cmdBuffer
        ) const = 0;

        const std::string& getName() const;

        const vk::PipelineStageFlags& getStageFlags() const;

        const std::vector<vk::AttachmentReference>& getColorAttachments() const;

        const std::vector<vk::AttachmentReference>& getUsedAttachments() const;

        const std::vector<vk::AttachmentReference>& getResolveAttachments() const;

        const std::vector<vk::AttachmentReference>& getDepthAttachments() const;
    };
}
#endif //UNNECESSARYENGINE_RENDER_PASS_H
