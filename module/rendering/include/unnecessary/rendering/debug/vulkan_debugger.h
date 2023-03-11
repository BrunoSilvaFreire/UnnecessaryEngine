//
// Created by bruno on 20/01/2022.
//

#ifndef UNNECESSARYENGINE_VULKAN_DEBUGGER_H
#define UNNECESSARYENGINE_VULKAN_DEBUGGER_H

#include <vulkan/vulkan.hpp>
#include <unnecessary/logging.h>

namespace un {
    class renderer;

    class vulkan_debugger {
    public:
        explicit vulkan_debugger(renderer& renderer);

    private:
        vk::Device _device;
        vk::DebugUtilsMessengerEXT _messenger;
        PFN_vkSetDebugUtilsObjectNameEXT _pSetDebugUtilsObjectName{};

        static VkBool32 messenger_callback(
            VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
            VkDebugUtilsMessageTypeFlagsEXT messageTypes,
            const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
            void* pUserData
        );

    public:
        template<class t_vulkan_cxx_type>
        void tag(
            t_vulkan_cxx_type obj,
            const std::string& tag
        ) const {
            vk::DebugUtilsObjectNameInfoEXT nameInfo(
                obj.objectType,
                static_cast<uint64_t>((typename t_vulkan_cxx_type::NativeType) obj),
                tag.c_str()
            );
            _pSetDebugUtilsObjectName(
                _device,
                &(nameInfo.operator const VkDebugUtilsObjectNameInfoEXT&())
            );
        }
    };
}
#endif
