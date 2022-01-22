#include <unnecessary/rendering/renderer.h>
#include <unnecessary/rendering/render_graph.h>
#include <unnecessary/rendering/render_pass.h>

namespace un {
    void RenderGraph::bake(Renderer& renderer) {
        vk::Device device = renderer.getVirtualDevice();
        std::vector<vk::SubpassDescription> subPasses;
        std::vector<vk::SubpassDependency> subPassesDependencies;
        subPasses.reserve(getSize());
        for (size_t i = 0; i < getSize(); ++i) {
            RenderPass* vertex = *(operator[](i));
            subPasses.emplace_back(
                (vk::SubpassDescriptionFlags) 0,
                vk::PipelineBindPoint::eGraphics,
                vertex->getUsedAttachments(),
                vertex->getColorAttachments(),
                vertex->getResolveAttachments(),
                vertex->getDepthAttachments().data()
            );
            for (auto[neighborIndex, edge] : dependenciesOf(i)) {
                auto neighbor = *operator[](neighborIndex);
                // TODO: Compute these
                vk::AccessFlags srcFlags;
                vk::AccessFlags dstFlags;
                vk::DependencyFlags dependencyFlags;
                subPassesDependencies.emplace_back(
                    i,
                    neighborIndex,
                    vertex->getStageFlags(),
                    neighbor->getStageFlags(),
                    srcFlags,
                    dstFlags,
                    dependencyFlags
                );
            }
        }
        std::vector<vk::AttachmentDescription> attachments;
        /*for (const auto& item: frameGraph.attachments) {
            attachments.emplace_back(item.getDescription());
        }*/

        vk::RenderPass createdPass = device.createRenderPass(
            vk::RenderPassCreateInfo(
                (vk::RenderPassCreateFlags) 0,
                attachments,
                subPasses,
                subPassesDependencies
            )
        );
        renderPass = createdPass;
        const auto& chain = renderer.getSwapChain();
        const auto& res = chain.getResolution();
        const auto& images = chain.getImages();
        for (int i = 0; i < images.size(); ++i) {
            frameBuffers.emplace_back(
                &renderer,
                *this,
                createdPass,
                i
            );
        }
    }

    bool RenderGraph::isAttachmentBorrowed(std::size_t index) const {
        return borrowedAttachments.contains(index);
    }

    const std::vector<un::Attachment>& RenderGraph::getAttachments() const {
        return attachments;
    }

    const std::vector<un::FrameBuffer>& RenderGraph::getFrameBuffers() const {
        return frameBuffers;
    }
    std::size_t RenderGraph::addBorrowedAttachment(
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
        borrowedAttachments.emplace(index);
        return index;
    }
    std::size_t RenderGraph::addOwnedAttachment(
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
            clearValue,
            initialLayout,
            usageFlags,
            aspectFlags
        );
        return index;
    }
}