#include <unnecessary/rendering/vulkan/vulkan_requirements.h>

namespace un {

    void VulkanQueueAvailableRequirement::check(
        const std::vector<vk::QueueFamilyProperties>& value,
        un::Validator& validator
    ) {
        bool isValid = std::any_of(
            value.begin(), value.end(), [&](const vk::QueueFamilyProperties& property) {
                return (property.queueFlags & bits) == bits;
            }
        );
        if (!isValid) {
            std::stringstream ss;
            ss << "Vulkan queue " << vk::to_string(bits) << " is not available on the device.";
            validator.addProblem(ss.str());
        }
    }

    VulkanQueueAvailableRequirement::VulkanQueueAvailableRequirement(
        vk::QueueFlagBits bits
    ) : bits(bits) { }

    void VulkanDeviceTypeRequirement::check(const vk::PhysicalDeviceProperties& value, un::Validator& validator) {

        if (value.deviceType != requiredType) {
            std::stringstream ss;
            ss << "Device " << value.deviceName << " is " << vk::to_string(value.deviceType);
            ss << ", but type " << vk::to_string(requiredType) << " is required.";
            validator.addProblem(ss.str());
        }
    }

    VulkanDeviceTypeRequirement::VulkanDeviceTypeRequirement(vk::PhysicalDeviceType requiredType) : requiredType(
        requiredType
    ) { }
}