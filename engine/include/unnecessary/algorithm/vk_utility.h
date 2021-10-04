#ifndef UNNECESSARYENGINE_VK_UTILITY_H
#define UNNECESSARYENGINE_VK_UTILITY_H

#include <vulkan/vulkan.hpp>
#include <unnecessary/graphics/rendering_device.h>

namespace un {
    vk::DeviceMemory allocateMemoryFor(
        const un::RenderingDevice& device,
        vk::MemoryPropertyFlags flags,
        vk::MemoryRequirements requirements
    );
}
#endif
