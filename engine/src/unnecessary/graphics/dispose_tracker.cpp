#include <unnecessary/graphics/dispose_tracker.h>

namespace un {
    void DisposeTracker::include(vk::DeviceMemory memory) {
        memoriesToFree.emplace(memory);
    }

    void DisposeTracker::include(vk::Buffer buffer) {
        buffersToDispose.emplace(buffer);
    }

    void DisposeTracker::dispose(vk::Device device) {
        for (vk::Buffer buf : buffersToDispose) {
            device.destroy(buf);
        }
        for (vk::DeviceMemory memory : memoriesToFree) {
            device.free(memory);
        }
    }
}