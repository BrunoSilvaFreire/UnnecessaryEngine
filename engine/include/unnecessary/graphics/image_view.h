
#ifndef UNNECESSARYENGINE_IMAGE_VIEW_H
#define UNNECESSARYENGINE_IMAGE_VIEW_H

#include <unnecessary/graphics/disposable.h>
#include <unnecessary/graphics/rendering_device.h>

namespace un {

    class ImageView : public IDisposable {
    private:
        vk::ImageView vulkanView;
    public:
        ImageView(
                un::RenderingDevice &renderer,
                vk::Image image,
                vk::Format format,
                vk::ImageViewType type = vk::ImageViewType::e2D
        );

        const vk::ImageView &getVulkanView() const;

        operator vk::ImageView();

        void dispose(vk::Device &device) override;
    };

}
#endif //UNNECESSARYENGINE_IMAGE_VIEW_H
