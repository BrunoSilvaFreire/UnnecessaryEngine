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

        _wrapped = device.createImageView(
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


    void ImageView::dispose(const vk::Device& device) {
        device.destroy(_wrapped);
    }
}