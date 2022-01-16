#include <unnecessary/graphics/dispose_tracker.h>

namespace un {
    void DisposeTracker::include(vk::DeviceMemory memory) {
        memoriesToFree.emplace(memory);
    }

    void DisposeTracker::include(vk::Buffer buffer) {
        buffersToDispose.emplace(buffer);
    }

    void DisposeTracker::dispose(const vk::Device& device) {
        for (vk::Buffer buf: buffersToDispose) {
            device.destroy(buf);
        }
        for (vk::DeviceMemory memory: memoriesToFree) {
            device.free(memory);
        }
        for (un::IDisposable* disposable: tracked) {
            disposable->dispose(device);
        }
    }

    void DisposeTracker::include(IDisposable* disposable) {
        tracked.emplace(disposable);
    }
}