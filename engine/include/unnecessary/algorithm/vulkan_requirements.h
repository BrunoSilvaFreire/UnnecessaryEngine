//
// Created by brunorbsf on 13/07/2020.
//

#ifndef UNNECESSARYENGINE_VULKAN_REQUIREMENTS_H
#define UNNECESSARYENGINE_VULKAN_REQUIREMENTS_H

#include <unnecessary/algorithm/requirements.h>
#include <vulkan/vulkan.hpp>
#include <vector>

namespace un {

    class VulkanQueueAvailableRequirement
        : public ARequirement<std::vector<vk::QueueFamilyProperties>> {
    private:
        vk::QueueFlagBits bits;
    public:
        explicit VulkanQueueAvailableRequirement(vk::QueueFlagBits bits);

        bool isMet(const std::vector<vk::QueueFamilyProperties>& value) override;
    };

    class VulkanDeviceTypeRequirement
        : public ARequirement<vk::PhysicalDeviceProperties> {
    private:
        vk::PhysicalDeviceType requiredType;
    public:
        explicit VulkanDeviceTypeRequirement(vk::PhysicalDeviceType requiredType);

        bool isMet(const vk::PhysicalDeviceProperties& value) override;
    };
}
#endif //UNNECESSARYENGINE_VULKAN_REQUIREMENTS_H
