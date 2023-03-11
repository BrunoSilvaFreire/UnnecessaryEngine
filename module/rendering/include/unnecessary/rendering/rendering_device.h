#ifndef UNNECESSARYENGINE_RENDERING_DEVICE_H
#define UNNECESSARYENGINE_RENDERING_DEVICE_H

#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>
#include <unnecessary/rendering/queue.h>

namespace un {

    class rendering_device {
    private:
        vk::Device _virtualDevice;
        vk::PhysicalDevice _physicalDevice;
        vk::SurfaceKHR _surface;
        un::queue _present;
        un::queue _graphics;
        vk::PhysicalDeviceMemoryProperties _memoryProperties;
        vk::PhysicalDeviceProperties _deviceProperties;

        rendering_device(
            const vk::Device& virtualDevice,
            const vk::PhysicalDevice& physicalDevice,
            const vk::SurfaceKHR& surface,
            const queue& present,
            const queue& graphics,
            const vk::PhysicalDeviceMemoryProperties& memoryProperties,
            const vk::PhysicalDeviceProperties& deviceProperties
        );

    public:

        static un::rendering_device create(
            const vk::Instance& vulkanInstance,
            GLFWwindow* window
        );

        u32 select_memory_type_for(
            const vk::MemoryRequirements& requirements,
            vk::MemoryPropertyFlags flags = static_cast<vk::MemoryPropertyFlags>(0)
        ) const;

        const vk::Device& get_virtual_device() const;

        const vk::PhysicalDevice& get_physical_device() const;

        const vk::PhysicalDeviceMemoryProperties& get_memory_properties() const;

        const vk::PhysicalDeviceProperties& get_device_properties() const;

        const vk::SurfaceKHR& get_surface() const;

        queue& get_graphics();

        queue& get_present();

        const queue& get_graphics() const;

        const queue& get_present() const;
    };

    vk::DeviceMemory allocate_memory_for(
        const rendering_device& device,
        vk::MemoryPropertyFlags flags,
        vk::MemoryRequirements requirements
    );

}
#endif
