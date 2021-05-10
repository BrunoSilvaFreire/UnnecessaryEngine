#ifndef UNNECESSARYENGINE_DISPOSABLE_H
#define UNNECESSARYENGINE_DISPOSABLE_H

#include <vulkan/vulkan.hpp>

namespace un {
    class IDisposable {
        virtual void dispose(vk::Device &device) = 0;
    };
}
#endif
