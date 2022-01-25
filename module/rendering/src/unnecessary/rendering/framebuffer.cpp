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
        std::string prefix = "FrameBuffer-";
        prefix += std::to_string(frameBufferIndex);
        for (std::size_t i = 0; i < attachments.size(); ++i) {
            auto attachment = attachments[i];
#ifdef DEBUG

            std::string suffix = "-";
            const std::string& name = attachment.getName();
            if (!name.empty()) {
                suffix += name;
            } else {
                suffix += std::to_string(i);
            };
#endif
            if (graph.isAttachmentBorrowed(i)) {
                vk::ImageView view = *chainImages[frameBufferIndex].getImageView();
                views.emplace_back(
                    view
                );
#ifdef DEBUG
                std::stringstream str;
                str << prefix << "-BorrowedView" << suffix;

                renderer->tag(view, str.str());
#endif
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
#ifdef DEBUG
                std::stringstream viewName;
                viewName << prefix << "-OwnedView" << suffix;
                renderer->tag(*imageView, viewName.str());
                std::stringstream imageName;
                imageName << prefix << "-OwnedImage" << suffix;
                image.tag(*renderer, imageName.str());
#endif
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
        renderer->tag(_wrapped, prefix);
    }
}