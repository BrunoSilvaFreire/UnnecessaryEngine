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
}