#ifndef UNNECESSARYENGINE_QUEUE_H
#define UNNECESSARYENGINE_QUEUE_H

#include <vulkan/vulkan.hpp>
#include <unnecessary/def.h>
#include <unnecessary/rendering/vulkan/vulkan_wrapper.h>

namespace un {
    class queue : public un::vulkan_wrapper<vk::Queue> {
    private:
        u32 _index;
    public:

        queue(u32 index, const vk::Queue& vulkan);

        u32 get_index() const;
    };
}

#endif
