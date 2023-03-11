#include <unnecessary/rendering/image_view.h>

namespace un {
    image_view::image_view(
        const rendering_device& renderer,
        vk::Image image,
        vk::Format format,
        vk::ImageViewType type,
        vk::ImageAspectFlags flags
    ) {
        vk::Device device = renderer.get_virtual_device();

        _wrapped = device.createImageView(
            vk::ImageViewCreateInfo(
                static_cast<vk::ImageViewCreateFlags>(0),
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

    void image_view::dispose(const vk::Device& device) {
        device.destroy(_wrapped);
    }
}
