#ifndef UNNECESSARYENGINE_RENDER_GRAPH_H
#define UNNECESSARYENGINE_RENDER_GRAPH_H

#include <vulkan/vulkan.hpp>
#include <unnecessary/graphs/dependency_graph.h>
#include <unnecessary/rendering/framebuffer.h>
#include <unnecessary/rendering/attachment.h>
#include <unnecessary/rendering/buffers/command_buffer.h>
#include <unnecessary/rendering/jobs/graphics_worker.h>
#include <optional>

namespace un {
    class render_pass;

    class renderer;

    struct pass_output {
        vk::Semaphore doneSemaphore;
        vk::PipelineStageFlags pipelineStage;
        vk::CommandBuffer passCommands;
    };

    /**
     * Contains information needed for _rendering a single frame
     */
    struct frame_data {
    public:
        std::vector<pass_output> passesOutputs;
        vk::RenderPass renderPass;
        vk::Framebuffer framebuffer;

        command_buffer request_command_buffer(
            graphics_worker* graphicsWorker,
            size_t renderPassIndex
        );
    };

    class render_graph : public dependency_graph<std::unique_ptr<render_pass>> {
    private:
        std::optional<vk::RenderPass> _renderPass;
        std::vector<frame_buffer> _frameBuffers;
        /**
          * Attachments identified by indices in this set already exists and
          * don't need to be created
          */
        std::set<u32> _borrowedAttachments;
        std::vector<attachment> _attachments;

    public:
        template<typename T, typename... Args>
        void enqueue_pass(Args... args) {
            enqueue_pass(std::make_unique<T>(args...));
        }

        void enqueue_pass(std::unique_ptr<render_pass>&& pass) {
            add(std::move(pass));
        }

        void bake(renderer& renderer);

        vk::RenderPass get_vulkan_pass() const {
            return *_renderPass;
        }

        vk::Framebuffer get_frame_buffer(std::size_t index) const {
            return *_frameBuffers[index];
        }

        std::size_t add_borrowed_attachment(
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

        std::size_t add_color_attachment(
            vk::ImageUsageFlags usageFlags,
            vk::ImageAspectFlags aspectFlags,
            const vk::ClearColorValue& clearValue,
            vk::AttachmentDescriptionFlags flags = {},
            vk::Format format = vk::Format::eUndefined,
            vk::SampleCountFlagBits samples = vk::SampleCountFlagBits::e1,
            vk::AttachmentLoadOp loadOp = vk::AttachmentLoadOp::eLoad,
            vk::AttachmentStoreOp storeOp = vk::AttachmentStoreOp::eStore,
            vk::ImageLayout initialLayout = vk::ImageLayout::eUndefined,
            vk::ImageLayout finalLayout = vk::ImageLayout::eUndefined
        );

        std::size_t add_owned_attachment(
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

        const attachment& get_attachment(std::size_t index) const;

        void set_attachment_name(size_t attachment, std::string name);

        bool is_attachment_borrowed(std::size_t index) const;

        const std::vector<frame_buffer>& get_frame_buffers() const;

        const std::vector<attachment>& get_attachments() const;
    };
};
#endif //UNNECESSARYENGINE_RENDER_GRAPH_H
