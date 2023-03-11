#ifndef UNNECESSARYENGINE_DISPOSABLE_H
#define UNNECESSARYENGINE_DISPOSABLE_H

#include <vulkan/vulkan.hpp>
#include <unnecessary/rendering/vulkan/vulkan_utils.h>
#include <string>

namespace un {
    class disposable {
    public:
        virtual void dispose(const vk::Device& device) = 0;
    };

}
#endif
