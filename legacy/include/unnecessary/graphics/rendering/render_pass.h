#ifndef UNNECESSARYENGINE_RENDER_PASS_H
#define UNNECESSARYENGINE_RENDER_PASS_H

#include <unnecessary/graphics/buffers/command_buffer.h>
#include <set>

namespace un {
    class Renderer;

    /**
     * This how the information required for creating a @see un::RenderPass
     */
    class RenderPassDescription {
        /**
         * This is not merged with RenderPassDescripTOR, because
         * in order to correctly configure this, we need to access information
         * on the Frame Graph, which uses this as it's node, and we do not want
         * to have cyclic references
         */
    private:
        std::string name;
        vk::PipelineStageFlags stageFlags;
        std::vector<vk::AttachmentReference> colorAttachments;
        std::vector<vk::AttachmentReference> usedAttachments;
        std::vector<vk::AttachmentReference> resolveAttachments;
        std::vector<vk::AttachmentReference> depthAttachments;
    public:
        RenderPassDescription(std::string name, vk::PipelineStageFlags stageFlags);

        const std::string& getName() const;


        const std::vector<vk::AttachmentReference>& getUsedAttachments() const;

        const std::vector<vk::AttachmentReference>& getColorAttachments() const;

        const std::vector<vk::AttachmentReference>& getResolveAttachments() const;

        const std::vector<vk::AttachmentReference>& getDepthAttachments() const;

        const vk::PipelineStageFlags& getStageFlags() const;

        friend class RenderPassDescriptor;
    };

    class RenderPass {
    private:
        un::CommandBuffer buffer;
        bool initialized;

        void checkInitialized() const;

    public:
        explicit RenderPass(un::Renderer* renderer);

        void end();

        void begin(
            const vk::CommandBufferInheritanceInfo* info
        );

        vk::CommandBuffer record();

        vk::CommandBuffer getCommandBuffer();
    };
};
#endif
