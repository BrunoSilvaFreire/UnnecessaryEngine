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

    class RenderPassInfo {
    private:
        std::string name;
        vk::PipelineStageFlags stageFlags;
        std::vector<vk::AttachmentReference> usedAttachments;
        std::vector<vk::AttachmentReference> colorAttachments;
        std::vector<vk::AttachmentReference> resolveAttachments;
        std::vector<vk::AttachmentReference> depthAttachments;
    public:
        RenderPassInfo(std::string name, vk::PipelineStageFlags stageFlags);

        const std::string& getName() const;


        const std::vector<vk::AttachmentReference>& getUsedAttachments() const;

        const std::vector<vk::AttachmentReference>& getColorAttachments() const;

        const std::vector<vk::AttachmentReference>& getResolveAttachments() const;

        const std::vector<vk::AttachmentReference>& getDepthAttachments() const;

        const vk::PipelineStageFlags& getStageFlags() const;

        friend class RenderPassDescriptor;
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
     *  @see un::RenderPassInfo
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

        u32 getPassIndex() const;

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

        un::RenderPassInfo* getPass();

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
        un::RenderPassInfo,
        un::RenderPassDependency,
        u32
    > {
    private:

        std::vector<FrameResource> resources;
        std::set<u32> independent;
        std::vector<un::Attachment> attachments;
    public:
        using VertexType = un::RenderPassInfo;
        using EdgeType = un::RenderPassDependency;
        using IndexType = u32;
        using EdgeView = gpp::AdjacencyList<un::RenderPassInfo, un::RenderPassDependency, u32>::EdgeView;

        friend class RenderPassDescriptor;

        friend class BakedFrameGraph;

        un::FrameResourceReference addResource(const std::string& resourceName);

        un::RenderPassDescriptor addPass(
            const std::string& passName,
            vk::PipelineStageFlags flags
        );

        un::RenderPassDescriptor addPass(
            const std::string& passName,
            vk::PipelineStageFlags flags,
            un::RenderPassInfo** result
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

    class RenderPass {
    private:
        un::CommandBuffer buffer;
        bool initialized;

        void checkInitialized() const {
            if (!initialized) {
                throw std::runtime_error("Buffer is not yet ready for recording.");
            }
        }

    public:
        explicit RenderPass(un::Renderer* renderer);

        void end() {
            checkInitialized();
            initialized = false;
            buffer->end();
        }

        void begin(
            const vk::CommandBufferInheritanceInfo* info
        ) {
            initialized = true;
            buffer->reset(vk::CommandBufferResetFlagBits::eReleaseResources);
            buffer->begin(
                vk::CommandBufferBeginInfo(
                    (vk::CommandBufferUsageFlags) vk::CommandBufferUsageFlagBits::eOneTimeSubmit |
                    vk::CommandBufferUsageFlagBits::eRenderPassContinue,
                    info
                )
            );
        }

        vk::CommandBuffer record() {
            checkInitialized();
            return getCommandBuffer();
        }


        vk::CommandBuffer getCommandBuffer() {
            return *buffer;
        }
    };

    class BakedFrameGraph {
    private:
        FrameGraph frameGraph;
        vk::RenderPass renderPass;
        std::vector<un::RenderPass> bakedPasses;
    public:
        BakedFrameGraph(un::FrameGraph&& graph, un::Renderer& renderer);

        const vk::RenderPass& getVulkanPass() const;

        un::FrameGraph& getFrameGraph();

        const un::FrameGraph& getFrameGraph() const;

        un::RenderPass& getRenderPass(u32 passIndex);

        un::RenderPass& getRenderPass(const un::RenderPassDescriptor& descriptor);
    };


}
#endif
