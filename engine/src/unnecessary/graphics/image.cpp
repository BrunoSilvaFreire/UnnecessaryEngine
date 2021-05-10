#include <unnecessary/graphics/image.h>

namespace un {


    Image::Image(
            un::RenderingDevice &renderer,
            vk::Format format,
            vk::Extent3D size,
            vk::ImageLayout layout,
            un::ImageDetails details,
            vk::ImageUsageFlagBits usage,
            vk::ImageType type,
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
    }
}