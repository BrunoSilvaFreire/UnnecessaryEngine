#ifndef UNNECESSARYENGINE_RENDERER_H
#define UNNECESSARYENGINE_RENDERER_H

#include <vulkan/vulkan.hpp>

namespace un {
    class Renderer {
    private:
        vk::PhysicalDevice physicalDevice;
        vk::Device virtualDevice;
        vk::Queue graphics;
    public:
        explicit Renderer(vk::Instance instance);

        vk::PhysicalDevice &getPhysicalDevice();

        vk::Device &getVirtualDevice();
    };
}


#endif //UNNECESSARYENGINE_RENDERER_H
