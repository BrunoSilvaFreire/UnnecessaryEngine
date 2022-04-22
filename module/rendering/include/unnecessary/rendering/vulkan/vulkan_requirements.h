//
// Created by brunorbsf on 13/07/2020.
//

#ifndef UNNECESSARYENGINE_VULKAN_REQUIREMENTS_H
#define UNNECESSARYENGINE_VULKAN_REQUIREMENTS_H

#include <unnecessary/rendering/vulkan/requirements.h>
#include <vulkan/vulkan.hpp>
#include <vector>

namespace un {

    class VulkanQueueAvailableRequirement
        : public Requirement<std::vector<vk::QueueFamilyProperties>> {
    private:
        vk::QueueFlagBits bits;
    public:
        explicit VulkanQueueAvailableRequirement(vk::QueueFlagBits bits);

        void check(const std::vector<vk::QueueFamilyProperties>& value, un::Validator& validator) override;
    };

    class VulkanDeviceTypeRequirement : public Requirement<vk::PhysicalDeviceProperties> {
    private:
        std::vector<vk::PhysicalDeviceType> acceptableTypes;
    public:
        explicit VulkanDeviceTypeRequirement(const std::initializer_list<vk::PhysicalDeviceType>& acceptableTypes);

        void check(const vk::PhysicalDeviceProperties& value, un::Validator& validator) override;
    };
}
#endif //UNNECESSARYENGINE_VULKAN_REQUIREMENTS_H
