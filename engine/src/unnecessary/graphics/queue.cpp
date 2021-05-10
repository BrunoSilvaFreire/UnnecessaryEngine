#include <unnecessary/graphics/queue.h>

namespace un {

    Queue::Queue(
            u32 index,
            const vk::Queue &vulkan
    ) : index(index), vulkan(vulkan) {}

    u32 Queue::getIndex() const {
        return index;
    }

    const vk::Queue &Queue::getVulkan() const {
        return vulkan;
    }

    Queue::Queue() = default;
}