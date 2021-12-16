

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
        vk::PhysicalDeviceProperties deviceProperties;
    public:
        RenderingDevice();

        RenderingDevice(
            const vk::Instance& vulkanInstance,
            GLFWwindow* window
        );

        u32 selectMemoryTypeFor(
            const vk::MemoryRequirements& requirements,
            vk::MemoryPropertyFlags flags = static_cast<vk::MemoryPropertyFlags>(0)
        ) const;

        const vk::Device& getVirtualDevice() const;

        const vk::PhysicalDevice& getPhysicalDevice() const;

        const vk::PhysicalDeviceMemoryProperties& getMemoryProperties() const;

        const vk::PhysicalDeviceProperties& getDeviceProperties() const;

        const vk::SurfaceKHR& getSurface() const;

        Queue& getGraphics();

        Queue& getPresent();

        const Queue& getGraphics() const;
    };
}
#endif
