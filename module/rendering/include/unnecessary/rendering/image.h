#ifndef UNNECESSARYENGINE_IMAGE_H
#define UNNECESSARYENGINE_IMAGE_H

#include <unnecessary/def.h>
#include <unnecessary/rendering/disposable.h>
#include <vulkan/vulkan.hpp>
#include <unnecessary/rendering/rendering_device.h>
#include <unnecessary/rendering/vulkan/vulkan_utils.h>

namespace un {
    class renderer;

    struct image_details {
    public:
        u8 mipLevels;
        vk::SampleCountFlagBits sampleCountFlag;
        vk::ImageTiling tiling;

        image_details(
            u8 mipLevels = 1,
            vk::SampleCountFlagBits sampleCountFlag = vk::SampleCountFlagBits::e1,
            vk::ImageTiling tiling = vk::ImageTiling::eOptimal

        );
    };

    class image : public disposable {
    private:
        vk::Image _image;
        vk::MemoryRequirements _memoryRequirements;
        vk::DeviceMemory _memory;

    public:
        image(
            const rendering_device& renderer,
            vk::Format format,
            vk::Extent3D size,
            vk::ImageLayout layout,
            vk::ImageUsageFlags usage,
            vk::ImageType type,
            image_details details = image_details(),
            vk::SharingMode mode = vk::SharingMode::eExclusive,
            vk::ImageCreateFlags flags = static_cast<vk::ImageCreateFlags>(0)
        );

        vk::Image operator*();

        operator vk::Image();

        void dispose(const vk::Device& device) override;

        void tag(
            renderer& renderer,
            const std::string& name
        );
    };
}
#endif
