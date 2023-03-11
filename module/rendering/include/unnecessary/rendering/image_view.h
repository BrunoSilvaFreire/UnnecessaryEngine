#ifndef UNNECESSARYENGINE_IMAGE_VIEW_H
#define UNNECESSARYENGINE_IMAGE_VIEW_H

#include <unnecessary/rendering/disposable.h>
#include <unnecessary/rendering/rendering_device.h>

namespace un {
    class image_view
        : public disposable,
          public vulkan_wrapper<vk::ImageView> {
    public:
        image_view(
            const rendering_device& renderer,
            vk::Image image,
            vk::Format format,
            vk::ImageViewType type = vk::ImageViewType::e2D,
            vk::ImageAspectFlags flags = vk::ImageAspectFlagBits::eColor
        );

        void dispose(const vk::Device& device) override;
    };
}
#endif //UNNECESSARYENGINE_IMAGE_VIEW_H
