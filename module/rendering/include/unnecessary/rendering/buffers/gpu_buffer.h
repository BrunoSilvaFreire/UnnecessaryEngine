#ifndef UNNECESSARYENGINE_GPU_BUFFER_H
#define UNNECESSARYENGINE_GPU_BUFFER_H

#include <vulkan/vulkan.hpp>

namespace un {
    class GPUBuffer {
    private:
        vk::Buffer value;
    public:
        vk::Buffer operator*() const {
            return value;
        }

        vk::Buffer operator->() const {
            return value;
        }
    };
}
#endif