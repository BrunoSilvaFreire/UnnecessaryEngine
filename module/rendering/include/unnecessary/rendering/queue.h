#ifndef UNNECESSARYENGINE_QUEUE_H
#define UNNECESSARYENGINE_QUEUE_H

#include <vulkan/vulkan.hpp>
#include <unnecessary/def.h>
#include <unnecessary/rendering/vulkan/vulkan_wrapper.h>

namespace un {
    class Queue : public un::VulkanWrapper<vk::Queue> {
    private:
        u32 index;
        vk::Queue vulkan;
    public:

        Queue(u32 index, const vk::Queue& vulkan);

        u32 getIndex() const;
    };
}


#endif
