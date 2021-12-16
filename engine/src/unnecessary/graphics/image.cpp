#include <unnecessary/graphics/image.h>
#include <unnecessary/algorithm/vk_utility.h>

namespace un {


    Image::Image(
        un::RenderingDevice& renderer,
        vk::Format format,
        vk::Extent3D size,
        vk::ImageLayout layout,
        vk::ImageUsageFlags usage,
        vk::ImageType type,
        un::ImageDetails details,
        vk::SharingMode mode,
        vk::ImageCreateFlags flags
    ) {
        vk::Device device = renderer.getVirtualDevice();
        u32 index = renderer.getGraphics().getIndex();
        image = device.createImage(
            vk::ImageCreateInfo(
                flags,
                type,
                format,
                size,
                details.mipLevels,
                1,
                details.sampleCountFlag,
                details.tiling,
                usage,
                mode,
                1,
                &index,
                layout
            )
        );
        memoryRequirements = device.getImageMemoryRequirements(image);
        memory = un::allocateMemoryFor(
            renderer,
            vk::MemoryPropertyFlagBits::eDeviceLocal,
            memoryRequirements
        );
        device.bindImageMemory(
            image,
            memory,
            0
        );
    }

    void Image::dispose(const vk::Device& device) {
        device.destroy(image);
    }

    vk::Image Image::operator*() {
        return image;
    }

    Image::operator vk::Image() {
        return image;
    }

    ImageDetails::ImageDetails(
        u8 mipLevels,
        vk::SampleCountFlagBits sampleCountFlag,
        vk::ImageTiling tiling
    ) : mipLevels(mipLevels), sampleCountFlag(sampleCountFlag), tiling(tiling) {}
}