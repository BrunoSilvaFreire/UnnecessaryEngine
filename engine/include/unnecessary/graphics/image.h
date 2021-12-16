#ifndef UNNECESSARYENGINE_IMAGE_H
#define UNNECESSARYENGINE_IMAGE_H

#include <unnecessary/def.h>
#include <unnecessary/graphics/disposable.h>
#include <vulkan/vulkan.hpp>
#include <unnecessary/graphics/rendering_device.h>
#include "unnecessary/algorithm/vk_utility.h"

namespace un {
    class Renderer;

    struct ImageDetails {
    public:
        u8 mipLevels;
        vk::SampleCountFlagBits sampleCountFlag;
        vk::ImageTiling tiling;

        ImageDetails(
            u8 mipLevels = 1,
            vk::SampleCountFlagBits sampleCountFlag = vk::SampleCountFlagBits::e1,
            vk::ImageTiling tiling = vk::ImageTiling::eOptimal

        );
    };

    class Image : public IDisposable {
    private:
        vk::Image image;
        vk::MemoryRequirements memoryRequirements;
        vk::DeviceMemory memory;
    public:
        Image(
            un::RenderingDevice& renderer,
            vk::Format format,
            vk::Extent3D size,
            vk::ImageLayout layout,
            vk::ImageUsageFlags usage,
            vk::ImageType type,
            un::ImageDetails details = un::ImageDetails(),
            vk::SharingMode mode = vk::SharingMode::eExclusive,
            vk::ImageCreateFlags flags = (vk::ImageCreateFlags) 0
        );

        vk::Image operator*();

        operator vk::Image();

        void dispose(const vk::Device& device) override;

        void tag(
            vk::Device device,
            const un::VulkanFunctionDatabase& db,
            const std::string& name
        ) {
            un::tag(image, device, db, name);
            un::tag(memory, device, db, name + "-Memory");
        }
    };

}
#endif
