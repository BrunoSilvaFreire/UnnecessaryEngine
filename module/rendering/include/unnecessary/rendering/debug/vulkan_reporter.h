//
// Created by bruno on 20/01/2022.
//

#ifndef UNNECESSARYENGINE_VULKAN_REPORTER_H
#define UNNECESSARYENGINE_VULKAN_REPORTER_H

#include <vulkan/vulkan.hpp>
#include <unnecessary/logging.h>

namespace un {
    class Renderer;

    class VulkanDebugger {
    public:

        VulkanDebugger(un::Renderer& renderer);

    private:
        vk::DebugUtilsMessengerEXT _messenger;

        static VkBool32 messenger_callback(
            VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
            VkDebugUtilsMessageTypeFlagsEXT messageTypes,
            const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
            void* pUserData
        );
    };
}
#endif
