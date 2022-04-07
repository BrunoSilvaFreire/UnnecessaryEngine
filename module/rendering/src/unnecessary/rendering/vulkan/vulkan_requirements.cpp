#include <unnecessary/rendering/vulkan/vulkan_requirements.h>

namespace un {
    bool
    VulkanQueueAvailableRequirement::isMet(const std::vector<vk::QueueFamilyProperties>& value) {
        return std::any_of(
            value.begin(), value.end(), [&](const vk::QueueFamilyProperties& property) {
                return (property.queueFlags & bits) == bits;
            }
        );
    }

    VulkanQueueAvailableRequirement::VulkanQueueAvailableRequirement(
        vk::QueueFlagBits bits
    ) : bits(bits) {}

    bool VulkanDeviceTypeRequirement::isMet(const vk::PhysicalDeviceProperties &value) {

        return value.deviceType == requiredType;
    }

    VulkanDeviceTypeRequirement::VulkanDeviceTypeRequirement(vk::PhysicalDeviceType requiredType) : requiredType(
        requiredType
    ) {}
}