
#ifndef UNNECESSARYENGINE_IMAGE_VIEW_H
#define UNNECESSARYENGINE_IMAGE_VIEW_H

#include <unnecessary/rendering/disposable.h>
#include <unnecessary/rendering/rendering_device.h>

namespace un {

    class ImageView : public IDisposable {
    private:
        vk::ImageView vulkanView;
    public:
        ImageView(
            un::RenderingDevice& renderer,
            vk::Image image,
            vk::Format format,
            vk::ImageViewType type = vk::ImageViewType::e2D,
            vk::ImageAspectFlags flags = vk::ImageAspectFlagBits::eColor
        );

        const vk::ImageView& getVulkanView() const;

        operator vk::ImageView();

        void dispose(const vk::Device& device) override;
    };

}
#endif //UNNECESSARYENGINE_IMAGE_VIEW_H
