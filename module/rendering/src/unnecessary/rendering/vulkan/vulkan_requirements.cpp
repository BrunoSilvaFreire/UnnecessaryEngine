#include <unnecessary/rendering/vulkan/vulkan_requirements.h>
#include <unnecessary/strings.h>

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

        bool compatible = std::any_of(acceptableTypes.begin(), acceptableTypes.end(),
                                      [&](vk::PhysicalDeviceType type) {
                                          return value.deviceType == type;
                                      }
        );
        if (!compatible) {
            std::stringstream ss;
            ss << "Device " << value.deviceName << " is " << vk::to_string(value.deviceType);
            ss << ", but only the types "
               << un::join_strings(acceptableTypes.begin(), acceptableTypes.end(), [](vk::PhysicalDeviceType type) {
                   return vk::to_string(type);
               })
               << " are acceptable.";
            validator.addProblem(ss.str());
        }
    }

    VulkanDeviceTypeRequirement::VulkanDeviceTypeRequirement(
        const std::initializer_list<vk::PhysicalDeviceType>& acceptableTypes
    ) : acceptableTypes(
        acceptableTypes
    ) { }
}