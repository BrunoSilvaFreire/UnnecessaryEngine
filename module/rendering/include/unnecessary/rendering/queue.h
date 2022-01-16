#ifndef UNNECESSARYENGINE_QUEUE_H
#define UNNECESSARYENGINE_QUEUE_H

#include <vulkan/vulkan.hpp>
#include <unnecessary/def.h>

namespace un {
    class Queue {
    private:
        u32 index;
        vk::Queue vulkan;
    public:

        Queue(u32 index, const vk::Queue& vulkan);

        u32 getIndex() const;

        const vk::Queue& getVulkan() const;

        vk::Queue* operator->();

        operator vk::Queue();
    };
}


#endif
