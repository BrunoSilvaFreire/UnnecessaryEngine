#ifndef UNNECESSARYENGINE_RENDER_GRAPH_H
#define UNNECESSARYENGINE_RENDER_GRAPH_H

#include <vulkan/vulkan.hpp>
#include <unnecessary/graphs/dependency_graph.h>
#include <unnecessary/rendering/framebuffer.h>
#include <unnecessary/rendering/attachment.h>

namespace un {
    class RenderPass;

    class Renderer;

    struct PassOutput {
        vk::Semaphore doneSemaphore;
        vk::PipelineStageFlags pipelineStage;
        vk::CommandBuffer passCommands;
    };
    /**
     * Contains information needed for _rendering a single frame
     */
    struct FrameData {
    public:
        std::vector<un::PassOutput> passesOutputs;
        vk::RenderPass renderPass;
        vk::Framebuffer framebuffer;
    };

    class RenderGraph : public un::DependencyGraph<un::RenderPass*> {
    private:
        std::optional<vk::RenderPass> renderPass;
        std::vector<un::FrameBuffer> frameBuffers;
        /**
          * Attachments identified by indices in this set already exists and
          * don't need to be created
          */
        std::set<u32> borrowedAttachments;
        std::vector<un::Attachment> attachments;
    public:
        template<typename T, typename ...Args>
        void enqueuePass(Args... args) {
            add(new T(args...));
        }

        void bake(un::Renderer& renderer);

        vk::RenderPass getVulkanPass() const {
            return *renderPass;
        }

        vk::Framebuffer getFrameBuffer(std::size_t index) {
            return frameBuffers[index];
        }

        std::size_t addBorrowedAttachment(
            const vk::ClearValue& clearValue,
            vk::AttachmentDescriptionFlags flags = {},
            vk::Format format = vk::Format::eUndefined,
            vk::SampleCountFlagBits samples = vk::SampleCountFlagBits::e1,
            vk::AttachmentLoadOp loadOp = vk::AttachmentLoadOp::eLoad,
            vk::AttachmentStoreOp storeOp = vk::AttachmentStoreOp::eStore,
            vk::AttachmentLoadOp stencilLoadOp = vk::AttachmentLoadOp::eLoad,
            vk::AttachmentStoreOp stencilStoreOp = vk::AttachmentStoreOp::eStore,
            vk::ImageLayout initialLayout = vk::ImageLayout::eUndefined,
            vk::ImageLayout finalLayout = vk::ImageLayout::eUndefined
        );

        std::size_t addOwnedAttachment(
            vk::ImageUsageFlags usageFlags,
            vk::ImageAspectFlags aspectFlags,
            const vk::ClearValue& clearValue,
            vk::AttachmentDescriptionFlags flags = {},
            vk::Format format = vk::Format::eUndefined,
            vk::SampleCountFlagBits samples = vk::SampleCountFlagBits::e1,
            vk::AttachmentLoadOp loadOp = vk::AttachmentLoadOp::eLoad,
            vk::AttachmentStoreOp storeOp = vk::AttachmentStoreOp::eStore,
            vk::AttachmentLoadOp stencilLoadOp = vk::AttachmentLoadOp::eLoad,
            vk::AttachmentStoreOp stencilStoreOp = vk::AttachmentStoreOp::eStore,
            vk::ImageLayout initialLayout = vk::ImageLayout::eUndefined,
            vk::ImageLayout finalLayout = vk::ImageLayout::eUndefined
        );

        bool isAttachmentBorrowed(size_t i) const;

        const std::vector<un::FrameBuffer>& getFrameBuffers() const;

        const std::vector<un::Attachment>& getAttachments() const;
    };
};
#endif //UNNECESSARYENGINE_RENDER_GRAPH_H
