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

    class RenderPass {
    private:
        std::string name;
        vk::PipelineStageFlags stageFlags;
        std::vector<vk::AttachmentReference> colorAttachments;
        std::vector<vk::AttachmentReference> usedAttachments;
        std::vector<vk::AttachmentReference> resolveAttachments;
        std::optional<vk::AttachmentReference> depthAttachment;
    protected:

        void usesColorAttachment(std::size_t attachmentIndex, vk::ImageLayout layout) {
            colorAttachments.emplace_back(static_cast<u32>(attachmentIndex), layout);
        }

        void usesDepthAttachment(std::size_t attachmentIndex, vk::ImageLayout layout) {
            depthAttachment = vk::AttachmentReference(static_cast<u32>(attachmentIndex), layout);
        }

    public:
        RenderPass(const std::string& name, const vk::PipelineStageFlags& stageFlags);

        virtual ~RenderPass() = default;

        virtual void onVulkanPassCreated(vk::RenderPass renderPass, un::Renderer& renderer) {

        }

        virtual void record(
            const un::FrameData& data,
            un::CommandBuffer& cmdBuffer
        ) const = 0;

        const std::string& getName() const;

        const vk::PipelineStageFlags& getStageFlags() const;

        const std::vector<vk::AttachmentReference>& getColorAttachments() const;

        const std::vector<vk::AttachmentReference>& getUsedAttachments() const;

        const std::vector<vk::AttachmentReference>& getResolveAttachments() const;

        const std::optional<vk::AttachmentReference>& getDepthAttachment() const;

        friend std::ostream& operator<<(std::ostream& os, const RenderPass& pass);

        friend std::ostream& operator<<(std::ostream& os, const RenderPass* pass);
    };
}
#endif //UNNECESSARYENGINE_RENDER_PASS_H
