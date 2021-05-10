

#ifndef UNNECESSARYENGINE_RENDERING_DEVICE_H
#define UNNECESSARYENGINE_RENDERING_DEVICE_H

#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>
#include <unnecessary/graphics/queue.h>

namespace un {
    class RenderingDevice {
    private:
        vk::Device virtualDevice;
        vk::PhysicalDevice physicalDevice;
        vk::SurfaceKHR surface;
        un::Queue graphics, present;
        vk::PhysicalDeviceMemoryProperties memoryProperties;
    public:
        RenderingDevice();

        RenderingDevice(
                const vk::Instance &vulkanInstance,
                GLFWwindow *window
        );

        u32 selectMemoryTypeFor(vk::Buffer buffer);

        const vk::Device &getVirtualDevice() const;

        const vk::PhysicalDevice &getPhysicalDevice() const;

        const vk::SurfaceKHR &getSurface() const;

        Queue &getGraphics();

        Queue &getPresent();
    };
}
#endif
