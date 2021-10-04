#ifndef UNNECESSARYENGINE_FRAME_GRAPH_H
#define UNNECESSARYENGINE_FRAME_GRAPH_H

#include <unnecessary/def.h>
#include <unnecessary/graphics/buffers/command_buffer.h>
#include <unnecessary/logging.h>
#include <grapphs/adjacency_list.h>
#include "attachment.h"
#include "frame_resource.h"
#include "renderpass.h"
#include <set>
#include <string>


namespace un {
    class FrameGraph;

    class BakedFrameGraph;

    class Renderer;


/**
 *  This is just a helper class to correctly configure a FrameGraph.
 *  If you wanna check out the actual class used as a vertex
 *  @see un::RenderPassDescription
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

        un::RenderPassDescription* getPass();

        void usesAttachment(std::size_t attachmentIndex, vk::ImageLayout layout);

        void usesColorAttachment(std::size_t attachmentIndex, vk::ImageLayout layout);

        void usesDepthAttachment(std::size_t attachmentIndex, vk::ImageLayout layout);
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

    class FrameGraph : public gpp::AdjacencyList<
        un::RenderPassDescription,
        un::RenderPassDependency,
        u32
    > {
    private:
        std::set<u32> independent;
        /**
         * Attachments identified by indices in this set already exists and
         * don't need to be created
         */
        std::set<u32> borrowedAttachments;
        std::vector<FrameResource> resources;
        std::vector<un::Attachment> attachments;
    public:
        using VertexType = un::RenderPassDescription;
        using EdgeType = un::RenderPassDependency;
        using IndexType = u32;
        using EdgeView = gpp::AdjacencyList<un::RenderPassDescription, un::RenderPassDependency, u32>::EdgeView;

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
            un::RenderPassDescription** result
        );

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

        const un::Attachment& getAttachment(std::size_t index) const;

        const std::set<u32>& getBorrowedAttachments() const;

        bool isAttachmentBorrowed(std::size_t index) const;

        const std::vector<un::Attachment>& getAttachments() const;
    };
}
#endif
