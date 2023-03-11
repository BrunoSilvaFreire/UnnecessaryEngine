#include <unnecessary/rendering/framebuffer.h>
#include <unnecessary/rendering/renderer.h>
#include <unnecessary/rendering/render_graph.h>

namespace un {
    frame_buffer::frame_buffer(
        renderer* renderer,
        const render_graph& graph,
        vk::RenderPass renderPass,
        std::size_t frameBufferIndex
    ) {
        auto device = renderer->get_virtual_device();
        const auto& chain = renderer->get_swap_chain();
        const size2d& resolution = chain.get_resolution();
        const rendering_device& renderingDevice = renderer->get_device();
        const auto& chainImages = chain.get_images();
        const auto& attachments = graph.get_attachments();
        std::vector<vk::ImageView> views;
        std::string prefix = "FrameBuffer-";
        prefix += std::to_string(frameBufferIndex);
        for (std::size_t i = 0; i < attachments.size(); ++i) {
            auto attachment = attachments[i];
#ifdef DEBUG

            std::string suffix = "-";
            const std::string& name = attachment.get_name();
            if (!name.empty()) {
                suffix += name;
            }
            else {
                suffix += std::to_string(i);
            }
#endif
            if (graph.is_attachment_borrowed(i)) {
                vk::ImageView view = *chainImages[frameBufferIndex].get_image_view();
                views.emplace_back(
                    view
                );
#ifdef DEBUG
                std::stringstream str;
                str << prefix << "-BorrowedView" << suffix;

                renderer->tag(view, str.str());
#endif
            }
            else {
                const vk::AttachmentDescription& description = attachment.get_description();
                vk::Format format = description.format;
                image& image = _ownedImages.emplace_back(
                    renderingDevice,
                    format,
                    vk::Extent3D(resolution.x, resolution.y, 1),
                    attachment.get_owned_image_layout(),
                    attachment.get_owned_image_usage(),
                    vk::ImageType::e2D
                );
                image_view& imageView = _ownedImagesView.emplace_back(
                    renderingDevice,
                    image,
                    format,
                    vk::ImageViewType::e2D,
                    attachment.get_owned_image_flags()
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
                static_cast<vk::FramebufferCreateFlags>(0),
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
