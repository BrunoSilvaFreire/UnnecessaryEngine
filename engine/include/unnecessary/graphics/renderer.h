#ifndef UNNECESSARYENGINE_RENDERER_H
#define UNNECESSARYENGINE_RENDERER_H

#include <vulkan/vulkan.hpp>
#include <unnecessary/def.h>

namespace un {
    class Renderer {
    private:
        vk::PhysicalDevice physicalDevice;
        vk::Device virtualDevice;
        u32 graphicsQueueIndex;
        vk::Queue graphics;
    public:
        explicit Renderer(vk::Instance instance);

        vk::PhysicalDevice &getPhysicalDevice();

        vk::Device &getVirtualDevice();

        vk::Queue &getGraphics();
    };
}


#endif //UNNECESSARYENGINE_RENDERER_H
