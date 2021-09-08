
#ifndef UNNECESSARYENGINE_DISPOSE_TRACKER_H
#define UNNECESSARYENGINE_DISPOSE_TRACKER_H

#include <set>
#include <vulkan/vulkan.hpp>

namespace un {
    class DisposeTracker {
    private:

        std::set<vk::Buffer> buffersToDispose;
        std::set<vk::DeviceMemory> memoriesToFree;
    public:

        void include(vk::Buffer buffer);

        void include(vk::DeviceMemory memory);

        void dispose(vk::Device device);
    };

}
#endif
