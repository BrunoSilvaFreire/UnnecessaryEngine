#include <unnecessary/rendering/renderer.h>
#include <unnecessary/rendering/debug/vulkan_debugger.h>
#include <unnecessary/rendering/vulkan/vulkan_utils.h>

#ifndef UN_VULKAN_DEBUG_PERFORMANCE
#define UN_VULKAN_DEBUG_PERFORMANCE true
#endif
#ifndef UN_VULKAN_DEBUG_SEVERITY
#define UN_VULKAN_DEBUG_SEVERITY eDebug
#endif

namespace un {
    vulkan_debugger::vulkan_debugger(
        renderer& renderer
    ) : _device(renderer.get_virtual_device()) {
        vk::Instance vulkan = renderer.get_vulkan();
        vk::DebugUtilsMessageSeverityFlagsEXT severity;
        auto maxSeverity = un::severity::UN_VULKAN_DEBUG_SEVERITY;
        if (maxSeverity <= severity::debug) {
            severity |= vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose;
        }
        if (maxSeverity <= severity::info) {
            severity |= vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo;
        }
        if (maxSeverity <= severity::warn) {
            severity |= vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning;
        }
        if (maxSeverity <= severity::error) {
            severity |= vk::DebugUtilsMessageSeverityFlagBitsEXT::eError;
        }
        vk::DebugUtilsMessageTypeFlagsEXT messageType;
        messageType |= vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral;
        messageType |= vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation;
        if constexpr (UN_VULKAN_DEBUG_PERFORMANCE) {
            messageType |= vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance;
        }
        PFN_vkCreateDebugUtilsMessengerEXT pfnCreateDebugUtilsMessengerEXT;
        if (try_find_function(
            vulkan,
            "vkCreateDebugUtilsMessengerEXT",
            &pfnCreateDebugUtilsMessengerEXT
        )) {
            vk::DebugUtilsMessengerCreateInfoEXT info(
                static_cast<vk::DebugUtilsMessengerCreateFlagsEXT>(0),
                severity,
                messageType,
                *reinterpret_cast<PFN_vkDebugUtilsMessengerCallbackEXT*>(messenger_callback),
                this
            );
            VK_CALL(
                static_cast<vk::Result>(pfnCreateDebugUtilsMessengerEXT(
                    vulkan,
                    reinterpret_cast<const VkDebugUtilsMessengerCreateInfoEXT*>(&info),
                    nullptr,
                    reinterpret_cast<VkDebugUtilsMessengerEXT*>(&_messenger)
                ))
            );
        }
        else {
            throw std::runtime_error(
                "Unable to find vkCreateDebugUtilsMessengerEXT function."
            );
        }
        load_function(
            vulkan,
            "vkSetDebugUtilsObjectNameEXT",
            &_pSetDebugUtilsObjectName
        );
    }

    VkBool32 vulkan_debugger::messenger_callback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageTypes,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData
    ) {
        auto debugger = static_cast<vulkan_debugger*>(pUserData);
        std::stringstream ostream;
        ostream << "Vulkan-"
                << vk::to_string(static_cast<vk::DebugUtilsMessageSeverityFlagBitsEXT>(messageSeverity))
                << "-"
                << vk::to_string(static_cast<vk::DebugUtilsMessageTypeFlagBitsEXT>(messageTypes))
                << ": " << pCallbackData->pMessage;
        switch (messageSeverity) {
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
                LOG(VERBOSE) << ostream.str();
                break;
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
                LOG(INFO) << ostream.str();
                break;
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
                LOG(WARN) << ostream.str();
                break;
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
                LOG(FUCK) << ostream.str();
#ifdef DEBUG
                debug_break();
#endif
                break;
        }
        return false;
    }
}
