#include <unnecessary/rendering/renderer.h>
#include <unnecessary/rendering/render_graph.h>
#include <unnecessary/rendering/render_pass.h>
#include <grapphs/algorithms/rlo_traversal.h>

namespace un {
    void render_graph::bake(renderer& renderer) {
        vk::Device device = renderer.get_virtual_device();
        std::vector<vk::SubpassDescription> subPasses;
        std::vector<vk::SubpassDependency> subPassesDependencies;
        subPasses.reserve(get_size());
        for (size_t i = 0; i < get_size(); ++i) {
            const auto& vertex = *(operator[](i));
            const std::optional<vk::AttachmentReference>& depthAttachment = vertex->get_depth_attachment();
            const vk::AttachmentReference* depth;
            if (depthAttachment.has_value()) {
                depth = depthAttachment.operator->();
            }
            else {
                depth = nullptr;
            }
            subPasses.emplace_back(
                static_cast<vk::SubpassDescriptionFlags>(0),
                vk::PipelineBindPoint::eGraphics,
                vertex->get_used_attachments(),
                vertex->get_color_attachments(),
                vertex->get_resolve_attachments(),
                depth
            );
            for (auto neighborIndex : dependencies_of(i)) {
                const auto& neighbor = *operator[](neighborIndex);
                // TODO: Compute these
                vk::AccessFlags srcFlags;
                vk::AccessFlags dstFlags;
                vk::DependencyFlags dependencyFlags;
                subPassesDependencies.emplace_back(
                    i,
                    neighborIndex,
                    vertex->get_stage_flags(),
                    neighbor->get_stage_flags(),
                    srcFlags,
                    dstFlags,
                    dependencyFlags
                );
            }
        }
        std::vector<vk::AttachmentDescription> passAttachments;
        for (const auto& attachment : _attachments) {
            passAttachments.emplace_back(attachment.get_description());
        }

        vk::RenderPass createdPass = device.createRenderPass(
            vk::RenderPassCreateInfo(
                static_cast<vk::RenderPassCreateFlags>(0),
                passAttachments,
                subPasses,
                subPassesDependencies
            )
        );
        _renderPass = createdPass;
        renderer.tag(createdPass, "Render Graph Pass");
        const auto& chain = renderer.get_swap_chain();
        const auto& res = chain.get_resolution();
        const auto& images = chain.get_images();
        for (int i = 0; i < images.size(); ++i) {
            _frameBuffers.emplace_back(
                &renderer,
                *this,
                createdPass,
                i
            );
        }
    }

    bool render_graph::is_attachment_borrowed(std::size_t index) const {
        return _borrowedAttachments.contains(static_cast<u32>(index));
    }

    const std::vector<attachment>& render_graph::get_attachments() const {
        return _attachments;
    }

    const std::vector<frame_buffer>& render_graph::get_frame_buffers() const {
        return _frameBuffers;
    }

    std::size_t render_graph::add_borrowed_attachment(
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
        std::size_t index = _attachments.size();
        _attachments.emplace_back(
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
        _borrowedAttachments.emplace(index);
        return index;
    }

    std::size_t render_graph::add_owned_attachment(
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
        std::size_t index = _attachments.size();
        _attachments.emplace_back(
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

    std::size_t render_graph::add_color_attachment(
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
        return add_owned_attachment(
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

    void render_graph::set_attachment_name(size_t attachment, std::string name) {
        _attachments[attachment].set_name(name);
    }

    const attachment& render_graph::get_attachment(std::size_t index) const {
        return _attachments[index];
    }

    command_buffer
    frame_data::request_command_buffer(
        graphics_worker* graphicsWorker,
        const size_t renderPassIndex
    ) {
        command_buffer buffer = graphicsWorker->request_command_buffer();
#if DEBUG
        std::stringstream stream;
        stream << "RenderPass-" << renderPassIndex << "-CommandBuffer";
        graphicsWorker->get_renderer()->tag(*buffer, stream.str());
#endif
        passesOutputs[renderPassIndex].passCommands = *buffer;
        return buffer;
    }
}
