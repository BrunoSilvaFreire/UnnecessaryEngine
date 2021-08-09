#include <unnecessary/graphics/buffer.h>

namespace un {
    Buffer::Buffer(
            Renderer &renderer,
            vk::BufferUsageFlags usage,
            u64 size,
            bool allocate,
            vk::MemoryPropertyFlags flags
    ) : size(size) {
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
            allocateMemory(renderer.getRenderingDevice(), flags);
        }
    }

    void Buffer::allocateMemory(const un::RenderingDevice &renderingDevice, vk::MemoryPropertyFlags flags) {
        const vk::Device &device = renderingDevice.getVirtualDevice();
        vk::MemoryRequirements requirements = device.getBufferMemoryRequirements(buffer);
        memory = device.allocateMemory(vk::MemoryAllocateInfo(
                requirements.size,
                renderingDevice.selectMemoryTypeFor(requirements, flags)
        ));
        device.bindBufferMemory(buffer, memory, 0);
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

    void Buffer::push(vk::Device device, void *data, bool flush) {
        void *toWriteTo;
        vkCall(
                device.mapMemory(
                        memory,
                        0,
                        size,
                        (vk::MemoryMapFlags) 0,
                        &toWriteTo
                )
        );
        std::memcpy(toWriteTo, data, size);
        if (flush) {
            device.flushMappedMemoryRanges(
                    {
                            vk::MappedMemoryRange(memory, 0, size)
                    }
            );
        }
        device.unmapMemory(memory);
    }


    vk::DeviceSize Buffer::getOffset() const {
        return offset;
    }

    vk::DeviceSize Buffer::getSize() const {
        return size;
    }

    CommandBuffer::CommandBuffer(Renderer &renderer) : CommandBuffer(renderer, renderer.getGlobalPool()) {

    }

    CommandBuffer::CommandBuffer(
            Renderer &renderer,
            vk::CommandPool pool
    ) : CommandBuffer(
            renderer,
            pool,
            (vk::CommandBufferUsageFlags) 0
    ) {

    }

    CommandBuffer::CommandBuffer(
            Renderer &renderer,
            vk::CommandPool pool,
            vk::CommandBufferUsageFlags flags
    ) : pool(pool) {
        device = renderer.getVirtualDevice();
        vk::CommandBufferAllocateInfo allocInfo(
                pool,
                vk::CommandBufferLevel::ePrimary,
                1
        );

        vkCall(
                device.allocateCommandBuffers(
                        &allocInfo,
                        &buffer
                )
        );
    }

    CommandBuffer::~CommandBuffer() {
        //renderer.freeCommandBuffers(pool, 1, &buffer);
    }

    vk::CommandBuffer CommandBuffer::getVulkanBuffer() {
        return buffer;
    }

    ResizableBuffer::ResizableBuffer() {}

    ResizableBuffer::ResizableBuffer(
            const vk::BufferUsageFlags &usage,
            u64 size,
            const vk::MemoryPropertyFlags &flags
    ) : bufferFlags(usage), memoryFlags(flags) {
        Buffer::size = size;
    }

    ResizableBuffer::ResizableBuffer(
            Renderer &renderer,
            const vk::BufferUsageFlags &usage,
            u64 size,
            bool allocate,
            const vk::MemoryPropertyFlags &flags
    ) : bufferFlags(usage), memoryFlags(flags) {
        createBuffer(renderer);
        if (allocate) {
            allocateMemory(renderer.getRenderingDevice(), flags);
        }
    }

    void ResizableBuffer::createBuffer(const Renderer &renderer) {
        vk::Device device = renderer.getVirtualDevice();
        if (buffer.operator!=(nullptr)) {
            device.destroyBuffer(buffer);
        }
        Queue graphics = renderer.getGraphics();
        u32 index = graphics.getIndex();
        buffer = device.createBuffer(
                vk::BufferCreateInfo(
                        (vk::BufferCreateFlags) 0,
                        size,
                        bufferFlags,
                        vk::SharingMode::eExclusive,
                        1,
                        &index
                )
        );
    }
}