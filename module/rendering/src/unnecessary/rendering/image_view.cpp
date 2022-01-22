#include <unnecessary/rendering/image_view.h>

namespace un {

    ImageView::ImageView(
        const un::RenderingDevice& renderer,
        vk::Image image,
        vk::Format format,
        vk::ImageViewType type,
        vk::ImageAspectFlags flags
    ) {
        vk::Device device = renderer.getVirtualDevice();

        vulkanView = device.createImageView(
            vk::ImageViewCreateInfo(
                (vk::ImageViewCreateFlags) 0,
                image,
                type,
                format,
                vk::ComponentMapping(),
                vk::ImageSubresourceRange(
                    flags,
                    0, 1, 0, 1
                )
            )
        );
    }

    const vk::ImageView& ImageView::getVulkanView() const {
        return vulkanView;
    }

    ImageView::operator vk::ImageView() {
        return vulkanView;
    }

    void ImageView::dispose(const vk::Device& device) {
        device.destroy(vulkanView);
    }
}