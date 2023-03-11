#include <unnecessary/rendering/queue.h>

namespace un {
    queue::queue(
        u32 index,
        const vk::Queue& vulkan
    ) : vulkan_wrapper<vk::Queue>(vulkan),
        _index(index) {
    }

    u32 queue::get_index() const {
        return _index;
    }
}
