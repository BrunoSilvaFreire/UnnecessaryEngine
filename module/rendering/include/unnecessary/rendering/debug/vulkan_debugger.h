//
// Created by bruno on 20/01/2022.
//

#ifndef UNNECESSARYENGINE_VULKAN_DEBUGGER_H
#define UNNECESSARYENGINE_VULKAN_DEBUGGER_H

#include <vulkan/vulkan.hpp>
#include <unnecessary/logging.h>

namespace un {
    class Renderer;

    class VulkanDebugger {
    public:

        VulkanDebugger(un::Renderer& renderer);

    private:
        vk::Device _device;
        vk::DebugUtilsMessengerEXT _messenger;
        PFN_vkSetDebugUtilsObjectNameEXT pSetDebugUtilsObjectName{};

        static VkBool32 messenger_callback(
            VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
            VkDebugUtilsMessageTypeFlagsEXT messageTypes,
            const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
            void* pUserData
        );

    public:
        template<class T>
        void tag(
            T obj,
            const std::string& tag
        ) const {
            vk::DebugUtilsObjectNameInfoEXT nameInfo(
                obj.objectType,
                (uint64_t) ((typename T::CType) obj),
                tag.c_str()
            );
            pSetDebugUtilsObjectName(
                _device,
                &(nameInfo.operator const VkDebugUtilsObjectNameInfoEXT&())
            );
        }
    };
}
#endif
