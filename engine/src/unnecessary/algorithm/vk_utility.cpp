#include <unnecessary/algorithm/vk_utility.h>

namespace un {

    vk::DeviceMemory allocateMemoryFor(
        const RenderingDevice& device,
        vk::MemoryPropertyFlags flags,
        vk::MemoryRequirements requirements
    ) {
        return device.getVirtualDevice().allocateMemory(
            vk::MemoryAllocateInfo(
                requirements.size,
                device.selectMemoryTypeFor(requirements, flags)
            )
        );
    }

    VulkanFunctionDatabase::VulkanFunctionDatabase(vk::Instance vulkan) {
        un::loadFunction(
            vulkan,
            "vkSetDebugUtilsObjectNameEXT",
            &pSetDebugUtilsObjectName
        );
    }

    void VulkanFunctionDatabase::setDebugUtilsObjectName(
        vk::Device device,
        const vk::DebugUtilsObjectNameInfoEXT& nameInfo
    ) const {
        pSetDebugUtilsObjectName(
            device,
            &(nameInfo.operator const VkDebugUtilsObjectNameInfoEXT&())
        );
    }
}