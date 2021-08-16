#ifndef UNNECESSARYENGINE_IMAGE_H
#define UNNECESSARYENGINE_IMAGE_H

#include <unnecessary/def.h>
#include <unnecessary/graphics/disposable.h>
#include <vulkan/vulkan.hpp>
#include <unnecessary/graphics/rendering_device.h>

namespace un {
    class Renderer;

    struct ImageDetails {
    public:
        u8 mipLevels = 1;
        vk::SampleCountFlagBits sampleCountFlag = vk::SampleCountFlagBits::e1;
        vk::ImageTiling tiling = vk::ImageTiling::eOptimal;
    };

    class Image : public IDisposable {
    private:
        vk::Image image;
    public:
        Image(
            un::RenderingDevice& renderer,
            vk::Format format,
            vk::Extent3D size,
            vk::ImageLayout layout,
            un::ImageDetails details,
            vk::ImageUsageFlagBits usage,
            vk::ImageType type,
            vk::SharingMode mode = vk::SharingMode::eExclusive,
            vk::ImageCreateFlags flags = (vk::ImageCreateFlags) 0
        );
//
//        Image(
//                un::Renderer &renderer,
//                vk::Format format,
//                un::Size2D size,
//                vk::ImageLayout layout,
//                vk::ImageCreateFlags flags = (vk::ImageCreateFlags) 0
//        )
//
//                :
//                Image(
//                        renderer,
//                        format,
//                        vk::Extent3D(size.x, size.y, 0),
//                        layout,
//                        vk::ImageType::e2D,
//                        flags
//                ) {
//        }
    };

}
#endif
