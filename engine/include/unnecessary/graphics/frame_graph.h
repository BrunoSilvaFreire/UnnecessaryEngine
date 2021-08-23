#ifndef UNNECESSARYENGINE_FRAME_GRAPH_H
#define UNNECESSARYENGINE_FRAME_GRAPH_H

#include <unnecessary/def.h>
#include <unnecessary/graphics/buffers/command_buffer.h>
#include <grapphs/adjacency_list.h>
#include <set>
#include <string>


namespace un {
    class FrameGraph;
    class BakedFrameGraph;

    class Renderer;

    class FrameResource {
    private:
        std::string name;
        std::set<u32> readPasses, writePasses;
    public:
        explicit FrameResource(std::string name);

        friend class RenderPassDescriptor;
    };

    class RenderPass {
    private:
        std::string name;
        vk::PipelineStageFlags stageFlags;
        std::vector<vk::AttachmentReference> usedAttachments;
        std::vector<vk::AttachmentReference> colorAttachments;
        std::vector<vk::AttachmentReference> resolveAttachments;
        std::vector<vk::AttachmentReference> depthAttachments;
        un::CommandBuffer commandBuffer;
    public:
        RenderPass(
            Renderer& renderer,
            std::string name,
            vk::PipelineStageFlags stageFlags
        );

        const std::string& getName() const;


        const std::vector<vk::AttachmentReference>& getUsedAttachments() const;

        const std::vector<vk::AttachmentReference>& getColorAttachments() const;

        const std::vector<vk::AttachmentReference>& getResolveAttachments() const;

        const std::vector<vk::AttachmentReference>& getDepthAttachments() const;

        const vk::PipelineStageFlags& getStageFlags() const;

        vk::CommandBuffer begin(
            const vk::CommandBufferInheritanceInfo* info,
            const un::BakedFrameGraph& frameGraph,
            vk::Rect2D renderArea,
            vk::Framebuffer framebuffer
        ) const;

        void end();

        friend class RenderPassDescriptor;

        vk::CommandBuffer getCommandBuffer() const;
    };

    class RenderPassDependency {
    public:
        enum Type {
            eProvides, eUses
        };
    private:
        Type type;
        vk::AccessFlags srcFlags, dstFlags;
        vk::DependencyFlags dependencyFlags;
    public:
        RenderPassDependency();

        RenderPassDependency(
            const vk::AccessFlags& srcFlags,
            const vk::AccessFlags& dstFlags,
            const vk::DependencyFlags& dependencyFlags
        );

        Type getType() const;

        const vk::AccessFlags& getSrcFlags() const;

        const vk::AccessFlags& getDstFlags() const;

        const vk::DependencyFlags& getDependencyFlags() const;
    };

    class FrameResourceReference {
    private:
        u32 index;
        FrameResource* resource;
    public:
        FrameResourceReference(u32 index, FrameResource* resource);

        u32 getIndex() const;

        FrameResource* getResource() const;
    };

    /**
     *  This is just a helper class to correctly configure a FrameGraph.
     *  If you wanna check out the actual class used as a vertex
     *  @see un::RenderPass
     */
    class RenderPassDescriptor {
    private:
        u32 passIndex;
        un::FrameGraph* frameGraph;

        bool tryFindResource(
            const un::FrameResourceReference& reference,
            un::FrameResource** resource
        );

    public:

        RenderPassDescriptor(u32 passIndex, FrameGraph* frameGraph);

        void writesTo(const un::FrameResourceReference& reference);

        void readsFrom(const un::FrameResourceReference& reference);

        void mustRunAfter(
            u32 pass,
            vk::AccessFlags srcFlags,
            vk::AccessFlags dstFlags,
            vk::DependencyFlags flags
        );

        void mustRunAfter(
            const RenderPassDescriptor& pass,
            vk::AccessFlags srcFlags,
            vk::AccessFlags dstFlags,
            vk::DependencyFlags flags
        );

        un::RenderPass* getPass();

        void usesAttachment(std::size_t attachmentIndex, vk::ImageLayout layout);

        void usesColorAttachment(std::size_t attachmentIndex, vk::ImageLayout layout);
    };

    class Attachment {
    private:
        vk::AttachmentDescription description;
        vk::ClearValue clear;
    public:
        Attachment(
            const vk::AttachmentDescription& description,
            const vk::ClearValue& clear
        );

        const vk::AttachmentDescription& getDescription() const;

        const vk::ClearValue& getClear() const;
    };

    class FrameGraph : public gpp::AdjacencyList<
        un::RenderPass,
        un::RenderPassDependency,
        u32
    > {
    private:
        std::vector<FrameResource> resources;
        std::set<u32> independent;
        std::vector<un::Attachment> attachments;
    public:
        using VertexType = un::RenderPass;
        using EdgeType = un::RenderPassDependency;
        using IndexType = u32;
        using EdgeView = gpp::AdjacencyList<un::RenderPass, un::RenderPassDependency, u32>::EdgeView;

        friend class RenderPassDescriptor;

        friend class BakedFrameGraph;

        un::FrameResourceReference addResource(const std::string& resourceName);

        un::RenderPassDescriptor addPass(
            un::Renderer& renderer,
            const std::string& passName,
            vk::PipelineStageFlags flags
        );

        un::RenderPassDescriptor addPass(
            un::Renderer& renderer,
            const std::string& passName,
            vk::PipelineStageFlags flags,
            un::RenderPass** result
        );

        std::size_t addAttachment(
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
        ) {
            std::size_t index = attachments.size();
            attachments.emplace_back(
                vk::AttachmentDescription(
                    flags,
                    format,
                    samples,
                    loadOp,
                    storeOp,
                    stencilLoadOp,
                    stencilStoreOp,
                    initialLayout,
                    finalLayout
                ),
                clearValue
            );
            return index;
        }

        std::size_t addAttachment(
            vk::ClearValue clearValue,
            const vk::AttachmentDescription& description
        ) {
            std::size_t index = attachments.size();
            attachments.emplace_back(description, clearValue);
            return index;
        }

        const std::vector<un::Attachment>& getAttachments() const;
    };

    class BakedFrameGraph {
    private:
        FrameGraph frameGraph;
        vk::RenderPass renderPass;
    public:
        BakedFrameGraph(un::FrameGraph&& graph, un::Renderer& renderer);

        const vk::RenderPass& getRenderPass() const;

        un::FrameGraph& getFrameGraph();

        const un::FrameGraph& getFrameGraph() const;
    };


}
#endif
