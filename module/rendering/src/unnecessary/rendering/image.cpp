#include <unnecessary/rendering/image.h>
#include <unnecessary/rendering/renderer.h>

namespace un {
    image::image(
        const rendering_device& renderer,
        vk::Format format,
        vk::Extent3D size,
        vk::ImageLayout layout,
        vk::ImageUsageFlags usage,
        vk::ImageType type,
        image_details details,
        vk::SharingMode mode,
        vk::ImageCreateFlags flags
    ) {
        vk::Device device = renderer.get_virtual_device();
        u32 index = renderer.get_graphics().get_index();
        _image = device.createImage(
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
        _memoryRequirements = device.getImageMemoryRequirements(_image);
        _memory = allocate_memory_for(
            renderer,
            vk::MemoryPropertyFlagBits::eDeviceLocal,
            _memoryRequirements
        );
        device.bindImageMemory(
            _image,
            _memory,
            0
        );
    }

    void image::dispose(const vk::Device& device) {
        device.destroy(_image);
    }

    vk::Image image::operator*() {
        return _image;
    }

    image::operator vk::Image() {
        return _image;
    }

    void image::tag(renderer& renderer, const std::string& name) {
        renderer.tag(_image, name);
        renderer.tag(_memory, name + "-Memory");
    }

    image_details::image_details(
        u8 mipLevels,
        vk::SampleCountFlagBits sampleCountFlag,
        vk::ImageTiling tiling
    ) : mipLevels(mipLevels), sampleCountFlag(sampleCountFlag), tiling(tiling) {
    }
}
