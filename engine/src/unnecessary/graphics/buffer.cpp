#include <unnecessary/graphics/buffer.h>

namespace un {
    Buffer::Buffer(Renderer &renderer, vk::BufferUsageFlags usage, u64 size, bool allocate) {
        vk::Device device = renderer.getVirtualDevice();
        un::Queue graphics = renderer.getGraphics();
        u32 index = graphics.getIndex();
        buffer = device.createBuffer(
                vk::BufferCreateInfo(
                        (vk::BufferCreateFlags) 0,
                        size,
                        usage,
                        vk::SharingMode::eExclusive,
                        1,
                        &index
                )
        );
        if (allocate) {
            memory = device.allocateMemory(vk::MemoryAllocateInfo(
                    size,
                    renderer.getRenderingDevice().selectMemoryTypeFor(buffer)
            ));
            device.bindBufferMemory(buffer, memory, 0);
        }
    }

    const vk::Buffer &Buffer::getVulkanBuffer() const {
        return buffer;
    }

    const vk::DeviceMemory &Buffer::getMemory() const {
        return memory;
    }

    Buffer::operator vk::Buffer &() {
        return buffer;
    }

    Buffer::operator vk::Buffer() const {
        return buffer;
    }

    CommandBuffer::CommandBuffer(Renderer &renderer) : CommandBuffer(renderer, renderer.getGlobalPool()) {

    }

    CommandBuffer::CommandBuffer(Renderer &renderer, vk::CommandPool pool) {
        device = renderer.getVirtualDevice();
        vk::CommandBufferAllocateInfo allocInfo(
                pool,
                vk::CommandBufferLevel::ePrimary,
                1
        );
        device.allocateCommandBuffers(
                &allocInfo,
                &buffer
        );
    }

    CommandBuffer::~CommandBuffer() {
        device.freeCommandBuffers(pool, 1, &buffer);
    }

    vk::CommandBuffer CommandBuffer::getVulkanBuffer() {
        return buffer;
    }
}