#include <unnecessary/rendering/vulkan/vulkan_requirements.h>
#include <unnecessary/strings.h>

namespace un {
    void vulkan_queue_available_requirement::check(
        const std::vector<vk::QueueFamilyProperties>& value,
        validator& validator
    ) {
        bool isValid = std::any_of(
            value.begin(), value.end(), [&](const vk::QueueFamilyProperties& property) {
                return (property.queueFlags & _bits) == _bits;
            }
        );
        if (!isValid) {
            std::stringstream ss;
            ss << "Vulkan queue " << vk::to_string(_bits) << " is not available on the device.";
            validator.add_problem(ss.str());
        }
    }

    vulkan_queue_available_requirement::vulkan_queue_available_requirement(
        vk::QueueFlagBits bits
    ) : _bits(bits) {
    }

    void
    vulkan_device_type_requirement::check(
        const vk::PhysicalDeviceProperties& value,
        validator& validator
    ) {
        bool compatible = std::any_of(
            _acceptableTypes.begin(), _acceptableTypes.end(),
            [&](vk::PhysicalDeviceType type) {
                return value.deviceType == type;
            }
        );
        if (!compatible) {
            std::stringstream ss;
            ss << "Device " << value.deviceName << " is " << vk::to_string(value.deviceType);
            ss << ", but only the types "
               << join_strings(
                   _acceptableTypes.begin(),
                   _acceptableTypes.end(),
                   [](vk::PhysicalDeviceType type) {
                       return vk::to_string(type);
                   }
               )
               << " are acceptable.";
            validator.add_problem(ss.str());
        }
    }

    vulkan_device_type_requirement::vulkan_device_type_requirement(
        const std::initializer_list<vk::PhysicalDeviceType>& acceptableTypes
    ) : _acceptableTypes(
        acceptableTypes
    ) {
    }
}
