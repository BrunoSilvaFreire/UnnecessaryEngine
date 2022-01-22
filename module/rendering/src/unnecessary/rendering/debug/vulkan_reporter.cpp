#include <unnecessary/rendering/renderer.h>
#include <unnecessary/rendering/debug/vulkan_reporter.h>
#include <unnecessary/rendering/vulkan/vulkan_utils.h>

#ifndef UN_VULKAN_DEBUG_PERFORMANCE
#define UN_VULKAN_DEBUG_PERFORMANCE true
#endif
#ifndef UN_VULKAN_DEBUG_SEVERITY
#define UN_VULKAN_DEBUG_SEVERITY eDebug
#endif
#ifdef WIN32
#define RAISE_BREAKPOINT __debugbreak()
#else
#endif
namespace un {

    VulkanDebugger::VulkanDebugger(un::Renderer& renderer) {
        vk::Instance vulkan = renderer.getVulkan();
        vk::DebugUtilsMessageSeverityFlagsEXT severity;
        un::Severity maxSeverity = un::Severity::UN_VULKAN_DEBUG_SEVERITY;
        if (maxSeverity <= un::Severity::eDebug) {
            severity |= vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose;
        }
        if (maxSeverity <= un::Severity::eInfo) {
            severity |= vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo;
        }
        if (maxSeverity <= un::Severity::eWarn) {
            severity |= vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning;
        }
        if (maxSeverity <= un::Severity::eError) {
            severity |= vk::DebugUtilsMessageSeverityFlagBitsEXT::eError;
        }
        vk::DebugUtilsMessageTypeFlagsEXT messageType;
        messageType |= vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral;
        messageType |= vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation;
        if (UN_VULKAN_DEBUG_PERFORMANCE) {
            messageType |= vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance;
        }
        PFN_vkCreateDebugUtilsMessengerEXT pfnCreateDebugUtilsMessengerEXT;
        if (un::tryFindFunction(
            vulkan,
            "vkCreateDebugUtilsMessengerEXT",
            &pfnCreateDebugUtilsMessengerEXT
        )) {
            vk::DebugUtilsMessengerCreateInfoEXT info(
                static_cast<vk::DebugUtilsMessengerCreateFlagsEXT>(0),
                severity,
                messageType,
                &VulkanDebugger::messenger_callback,
                this
            );
            vkCall(
                static_cast<vk::Result>(pfnCreateDebugUtilsMessengerEXT(
                    vulkan,
                    reinterpret_cast<const VkDebugUtilsMessengerCreateInfoEXT*>(&info),
                    nullptr,
                    reinterpret_cast<VkDebugUtilsMessengerEXT*>(&_messenger)
                ))
            );
        } else {

            throw std::runtime_error(
                "Unable to find vkCreateDebugUtilsMessengerEXT function."
            );
        }

    }

    VkBool32 VulkanDebugger::messenger_callback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageTypes,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData
    ) {
        auto debugger = static_cast<un::VulkanDebugger*>(pUserData);
        std::stringstream ostream;
        ostream << "Vulkan Error-"
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
                RAISE_BREAKPOINT;
                break;
        }
        return false;
    }
}