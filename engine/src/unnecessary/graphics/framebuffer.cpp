#include <unnecessary/graphics/framebuffer.h>
#include <unnecessary/graphics/renderer.h>
#include <unnecessary/graphics/rendering/baked_frame_graph.h>

namespace un {

    FrameBuffer::FrameBuffer(
        un::Renderer* renderer,
        const FrameGraph& graph,
        vk::RenderPass renderPass,
        std::size_t frameBufferIndex
    ) {
        auto device = renderer->getVirtualDevice();
        const auto& chain = renderer->getSwapChain();
        const un::Size2D& resolution = chain.getResolution();
        std::vector<vk::ImageView> views;
        un::RenderingDevice& renderingDevice = renderer->getRenderingDevice();

        const auto& attachments = graph.getAttachments();
        for (std::size_t i = 0; i < attachments.singleSize(); ++i) {
            auto attachment = attachments[i];

            if (graph.isAttachmentBorrowed(i)) {
                views.emplace_back(
                    chain.getViews()[frameBufferIndex].getVulkanView()
                );
            } else {
                const vk::AttachmentDescription& description = attachment.getDescription();
                vk::Format format = description.format;
                un::Image& image = ownedImages.emplace_back(
                    renderingDevice,
                    format,
                    vk::Extent3D(resolution.x, resolution.y, 1),
                    attachment.getOwnedImageLayout(),
                    attachment.getOwnedImageUsage(),
                    vk::ImageType::e2D
                );
                un::ImageView& imageView = ownedImagesView.emplace_back(
                    renderingDevice,
                    image,
                    format,
                    vk::ImageViewType::e2D,
                    attachment.getOwnedImageFlags()
                );
                views.emplace_back(imageView);
            }
        }
        framebuffer = device.createFramebuffer(
            vk::FramebufferCreateInfo(
                (vk::FramebufferCreateFlags) 0,
                renderPass,
                views,
                resolution.x,
                resolution.y,
                1
            )
        );
    }

    FrameBuffer::operator vk::Framebuffer() {
        return framebuffer;
    }

    void FrameBuffer::tag(
        vk::Device device,
        const VulkanFunctionDatabase& db,
        const std::string& name
    ) {
        un::tag(framebuffer, device, db, name);
    }
}