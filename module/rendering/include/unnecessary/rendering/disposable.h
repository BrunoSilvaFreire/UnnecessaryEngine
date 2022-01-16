#ifndef UNNECESSARYENGINE_DISPOSABLE_H
#define UNNECESSARYENGINE_DISPOSABLE_H

#include <vulkan/vulkan.hpp>
#include <unnecessary/rendering/vulkan/vulkan_utils.h>
#include <string>

namespace un {
    class IDisposable {
    public:
        virtual void dispose(const vk::Device& device) = 0;
    };

    class ITaggable {
        virtual void tag(
            vk::Device device,
            const un::VulkanFunctionDatabase& db,
            const std::string& name
        ) = 0;
    };

}
#endif
