#include <unnecessary/rendering/queue.h>

namespace un {

    Queue::Queue(
        u32 index,
        const vk::Queue& vulkan
    ) : index(index),
        un::VulkanWrapper<vk::Queue>(vulkan) {

    }

    u32 Queue::getIndex() const {
        return index;
    }
}