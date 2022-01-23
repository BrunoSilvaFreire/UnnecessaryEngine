#include <unnecessary/rendering/framebuffer.h>
#include <unnecessary/rendering/renderer.h>
#include <unnecessary/rendering/render_graph.h>

namespace un {

    FrameBuffer::FrameBuffer(
        un::Renderer* renderer,
        const un::RenderGraph& graph,
        vk::RenderPass renderPass,
        std::size_t frameBufferIndex
    ) {
        auto device = renderer->getVirtualDevice();
        const auto& chain = renderer->getSwapChain();
        const un::Size2D& resolution = chain.getResolution();
        const un::RenderingDevice& renderingDevice = renderer->getDevice();
        const auto& chainImages = chain.getImages();
        const auto& attachments = graph.getAttachments();
        std::vector<vk::ImageView> views;
        for (std::size_t i = 0; i < attachments.size(); ++i) {
            auto attachment = attachments[i];

            if (graph.isAttachmentBorrowed(i)) {
                views.emplace_back(
                    *chainImages[frameBufferIndex].getImageView()
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
        _wrapped = device.createFramebuffer(
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
}