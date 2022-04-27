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
            const std::optional<vk::AttachmentReference>& depthAttachment = vertex->getDepthAttachment();
            const vk::AttachmentReference* depth;
            if (depthAttachment.has_value()) {
                depth = depthAttachment.operator->();
            } else {
                depth = nullptr;
            }
            subPasses.emplace_back(
                (vk::SubpassDescriptionFlags) 0,
                vk::PipelineBindPoint::eGraphics,
                vertex->getUsedAttachments(),
                vertex->getColorAttachments(),
                vertex->getResolveAttachments(),
                depth
            );
            for (auto neighborIndex : dependenciesOf(i)) {
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
        std::vector<vk::AttachmentDescription> passAttachments;
        for (const auto& attachment : attachments) {
            passAttachments.emplace_back(attachment.getDescription());
        }

        vk::RenderPass createdPass = device.createRenderPass(
            vk::RenderPassCreateInfo(
                (vk::RenderPassCreateFlags) 0,
                passAttachments,
                subPasses,
                subPassesDependencies
            )
        );
        renderPass = createdPass;
        renderer.tag(createdPass, "Render Graph Pass");
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
        return borrowedAttachments.contains(static_cast<u32>(index));
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

    std::size_t RenderGraph::addColorAttachment(
        vk::ImageUsageFlags usageFlags,
        vk::ImageAspectFlags aspectFlags,
        const vk::ClearColorValue& clearValue,
        vk::AttachmentDescriptionFlags flags,
        vk::Format format,
        vk::SampleCountFlagBits samples,
        vk::AttachmentLoadOp loadOp,
        vk::AttachmentStoreOp storeOp,
        vk::ImageLayout initialLayout,
        vk::ImageLayout finalLayout
    ) {
        return addOwnedAttachment(
            usageFlags,
            aspectFlags,
            clearValue,
            flags, format,
            samples,
            loadOp,
            storeOp,
            vk::AttachmentLoadOp::eDontCare,
            vk::AttachmentStoreOp::eDontCare,
            initialLayout,
            finalLayout
        );
    }

    void RenderGraph::setAttachmentName(size_t attachment, std::string name) {
        attachments[attachment].setName(name);
    }

    un::CommandBuffer
    FrameData::requestCommandBuffer(un::GraphicsWorker* graphicsWorker, const size_t renderPassIndex) {
        un::CommandBuffer buffer = graphicsWorker->requestCommandBuffer();
#if DEBUG
        std::stringstream stream;
        stream << "RenderPass-" << renderPassIndex << "-CommandBuffer";
        graphicsWorker->getRenderer()->tag(*buffer, stream.str());
#endif
        passesOutputs[renderPassIndex].passCommands = *buffer;
        return buffer;
    }
}