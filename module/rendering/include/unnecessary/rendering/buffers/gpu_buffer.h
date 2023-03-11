#ifndef UNNECESSARYENGINE_GPU_BUFFER_H
#define UNNECESSARYENGINE_GPU_BUFFER_H

#include <vulkan/vulkan.hpp>

namespace un {
    class gpu_buffer {
    private:
        vk::Buffer _value;
    public:
        vk::Buffer operator*() const {
            return _value;
        }

        vk::Buffer operator->() const {
            return _value;
        }
    };
}
#endif