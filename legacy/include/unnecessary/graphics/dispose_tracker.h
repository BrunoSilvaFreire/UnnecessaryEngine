
#ifndef UNNECESSARYENGINE_DISPOSE_TRACKER_H
#define UNNECESSARYENGINE_DISPOSE_TRACKER_H

#include <set>
#include <vulkan/vulkan.hpp>
#include <unnecessary/graphics/disposable.h>

namespace un {
    class DisposeTracker : IDisposable {
    private:

        std::set<vk::Buffer> buffersToDispose;
        std::set<vk::DeviceMemory> memoriesToFree;
        std::set<IDisposable*> tracked;
    public:

        void include(vk::Buffer buffer);

        void include(vk::DeviceMemory memory);

        void include(IDisposable* disposable);

        void dispose(const vk::Device& device) override;
    };

}
#endif
