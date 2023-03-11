//
// Created by brunorbsf on 13/07/2020.
//

#ifndef UNNECESSARYENGINE_VULKAN_REQUIREMENTS_H
#define UNNECESSARYENGINE_VULKAN_REQUIREMENTS_H

#include <unnecessary/rendering/vulkan/requirements.h>
#include <vulkan/vulkan.hpp>
#include <vector>

namespace un {

    class vulkan_queue_available_requirement
        : public requirement<std::vector<vk::QueueFamilyProperties>> {
    private:
        vk::QueueFlagBits _bits;
    public:
        explicit vulkan_queue_available_requirement(vk::QueueFlagBits bits);

        void
        check(
            const std::vector<vk::QueueFamilyProperties>& value,
            un::validator& validator
        ) override;
    };

    class vulkan_device_type_requirement : public requirement<vk::PhysicalDeviceProperties> {
    private:
        std::vector<vk::PhysicalDeviceType> _acceptableTypes;
    public:
        vulkan_device_type_requirement(const std::initializer_list<vk::PhysicalDeviceType>& acceptableTypes);

        void check(const vk::PhysicalDeviceProperties& value, un::validator& validator) override;
    };
}
#endif //UNNECESSARYENGINE_VULKAN_REQUIREMENTS_H
