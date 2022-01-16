#include <unnecessary/rendering/queue.h>

namespace un {

    Queue::Queue(
        u32 index,
        const vk::Queue& vulkan
    ) : index(index), vulkan(vulkan) {}

    u32 Queue::getIndex() const {
        return index;
    }

    const vk::Queue& Queue::getVulkan() const {
        return vulkan;
    }

    vk::Queue* Queue::operator->() {
        return &vulkan;
    }

    Queue::operator vk::Queue() {
        return vulkan;
    }
}