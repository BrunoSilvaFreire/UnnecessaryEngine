#include "unnecessary/graphics/rendering/frame_graph.h"

#include <utility>
#include <unnecessary/logging.h>
#include <unnecessary/graphics/renderer.h>
#include <unnecessary/misc/aggregator.h>
#include <unnecessary/graphics/rendering/rendering_pipeline.h>

namespace un {

    void RenderPassDescriptor::writesTo(const FrameResourceReference& reference) {
        un::FrameResource* resource;
        if (!tryFindResource(reference, &resource)) {
            un::RenderPassDescription* pPass = frameGraph->vertex(passIndex);
            LOG(FUCK) << "Render pass '" << GREEN(pPass->getName())
                      << "' tried to write to resource " <<
                      reference.getIndex()
                      << ", which is not found in the frame graph it lives in.";
            return;
        }
        resource->writePasses.emplace(passIndex);
        for (u32 pass: resource->readPasses) {
            frameGraph->independent.erase(pass);
        }
    }

    const un::Attachment& FrameGraph::getAttachment(std::size_t index) const {
        return attachments[index];
    }

    void RenderPassDescriptor::readsFrom(const FrameResourceReference& reference) {
        un::FrameResource* resource;
        if (!tryFindResource(reference, &resource)) {
            un::RenderPassDescription* pPass = frameGraph->vertex(passIndex);
            LOG(FUCK) << "Render pass '" << GREEN(pPass->getName())
                      << "' tried to read from resource " <<
                      reference.getIndex()
                      << ", which is not found in the frame graph it lives in.";
            return;
        }
        resource->readPasses.emplace(passIndex);
        std::set<u32>& passes = resource->writePasses;
        if (!passes.empty()) {
            frameGraph->independent.erase(passIndex);
            for (u32 writer: passes) {
                mustRunAfter(
                    writer,
                    vk::AccessFlagBits::eMemoryRead,
                    vk::AccessFlagBits::eNoneKHR,
                    static_cast<vk::DependencyFlags>(0)
                );
            }
        }
    }

    RenderPassDescriptor::RenderPassDescriptor(
        u32 passIndex,
        FrameGraph* frameGraph
    ) : passIndex(passIndex), frameGraph(frameGraph) {}

    bool RenderPassDescriptor::tryFindResource(
        const FrameResourceReference& reference,
        un::FrameResource** resource
    ) {
        auto index = reference.getIndex();
        auto resources = frameGraph->resources;
        if (index >= resources.singleSize()) {
            *resource = nullptr;
            return false;
        } else {
            *resource = &resources[index];
            return true;
        }
    }

    un::RenderPassDescription* RenderPassDescriptor::getPass() {
        return frameGraph->vertex(passIndex);
    }

    void RenderPassDescriptor::mustRunAfter(
        u32 pass,
        vk::AccessFlags srcFlags,
        vk::AccessFlags dstFlags,
        vk::DependencyFlags flags
    ) {
        frameGraph->connect(
            pass, passIndex,
            un::RenderPassDependency()
        );
        frameGraph->connect(
            passIndex, pass,
            un::RenderPassDependency()
        );
    }

    void RenderPassDescriptor::mustRunAfter(
        const RenderPassDescriptor& pass,
        vk::AccessFlags srcFlags,
        vk::AccessFlags dstFlags,
        vk::DependencyFlags flags
    ) {
        mustRunAfter(pass.passIndex, srcFlags, dstFlags, flags);
    }

    void RenderPassDescriptor::usesAttachment(
        std::size_t attachmentIndex,
        vk::ImageLayout layout
    ) {
        getPass()->usedAttachments.emplace_back(attachmentIndex, layout);
    }

    void RenderPassDescriptor::usesColorAttachment(
        std::size_t attachmentIndex,
        vk::ImageLayout layout
    ) {
        getPass()->colorAttachments.emplace_back(attachmentIndex, layout);
    }

    void RenderPassDescriptor::usesDepthAttachment(
        std::size_t attachmentIndex,
        vk::ImageLayout layout
    ) {
        getPass()->depthAttachments.emplace_back(attachmentIndex, layout);
    }

    u32 RenderPassDescriptor::getPassIndex() const {
        return passIndex;
    }

    const std::string& RenderPassDescription::getName() const {
        return name;
    }

    u32 FrameResourceReference::getIndex() const {
        return index;
    }

    FrameResource* FrameResourceReference::getResource() const {
        return resource;
    }

    std::size_t FrameGraph::addBorrowedAttachment(
        const vk::ClearValue& clearValue,
        vk::AttachmentDescriptionFlags flags,
        vk::Format format,
        vk::SampleCountFlagBits samples,
        vk::AttachmentLoadOp loadOp,
        vk::AttachmentStoreOp storeOp,
        vk::AttachmentLoadOp stencilLoadOp,
        vk::AttachmentStoreOp stencilStoreOp,
        vk::ImageLayout initialLayout,
        vk::ImageLayout finalLayout
    ) {
        std::size_t index = attachments.singleSize();
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
        borrowedAttachments.emplace(index);
        return index;
    }

    std::size_t FrameGraph::addOwnedAttachment(
        vk::ImageUsageFlags usageFlags,
        vk::ImageAspectFlags aspectFlags,
        const vk::ClearValue& clearValue,
        vk::AttachmentDescriptionFlags flags,
        vk::Format format,
        vk::SampleCountFlagBits samples,
        vk::AttachmentLoadOp loadOp,
        vk::AttachmentStoreOp storeOp,
        vk::AttachmentLoadOp stencilLoadOp,
        vk::AttachmentStoreOp stencilStoreOp,
        vk::ImageLayout initialLayout,
        vk::ImageLayout finalLayout
    ) {
        std::size_t index = attachments.singleSize();
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
            clearValue,
            initialLayout,
            usageFlags,
            aspectFlags
        );
        return index;
    }

    FrameResourceReference::FrameResourceReference(
        u32 index,
        FrameResource* resource
    ) : index(index), resource(resource) {}


    const vk::PipelineStageFlags& RenderPassDescription::getStageFlags() const {
        return stageFlags;
    }


    RenderPassDependency::Type RenderPassDependency::getType() const {
        return type;
    }

    RenderPassDependency::RenderPassDependency(
        const vk::AccessFlags& srcFlags,
        const vk::AccessFlags& dstFlags,
        const vk::DependencyFlags& dependencyFlags
    ) : type(un::RenderPassDependency::Type::eUses),
        srcFlags(srcFlags),
        dstFlags(dstFlags),
        dependencyFlags(dependencyFlags) {}

    RenderPassDependency::RenderPassDependency() : type(un::RenderPassDependency::Type::eProvides) {}

    const vk::AccessFlags& RenderPassDependency::getSrcFlags() const {
        return srcFlags;
    }

    const vk::AccessFlags& RenderPassDependency::getDstFlags() const {
        return dstFlags;
    }

    const vk::DependencyFlags& RenderPassDependency::getDependencyFlags() const {
        return dependencyFlags;
    }


}