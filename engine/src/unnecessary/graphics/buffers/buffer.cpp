#include <unnecessary/graphics/buffers/buffer.h>
#include <unnecessary/graphics/buffers/command_buffer.h>
#include <unnecessary/algorithm/vk_utility.h>

namespace un {
    Buffer::Buffer(
        Renderer& renderer,
        vk::BufferUsageFlags usage,
        u64 size,
        bool allocate,
        vk::MemoryPropertyFlags flags
    ) : size(size), offset(0) {
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

    void Buffer::allocateMemory(
        const un::RenderingDevice& renderingDevice,
        vk::MemoryPropertyFlags flags
    ) {
        const vk::Device& device = renderingDevice.getVirtualDevice();
        auto deviceProperties = renderingDevice.getDeviceProperties();
        memoryRequirements = device.getBufferMemoryRequirements(buffer);
        memory = un::allocateMemoryFor(renderingDevice, flags, memoryRequirements);
        device.bindBufferMemory(buffer, memory, 0);
    }

    const vk::Buffer& Buffer::getVulkanBuffer() const {
        return buffer;
    }

    const vk::DeviceMemory& Buffer::getMemory() const {
        return memory;
    }

    Buffer::operator vk::Buffer&() {
        return buffer;
    }

    Buffer::operator vk::Buffer() const {
        return buffer;
    }

    void Buffer::push(vk::Device device, void* data, bool flush) {
        void* toWriteTo;
        vkCall(
            device.mapMemory(
                memory,
                0,
                memoryRequirements.size,
                (vk::MemoryMapFlags) 0,
                &toWriteTo
            )
        );
        std::memcpy(toWriteTo, data, size);
        if (flush) {
            std::array<vk::MappedMemoryRange, 1> ranges;
            ranges.front() = vk::MappedMemoryRange(memory, 0, VK_WHOLE_SIZE);
            vkCall(
                device.flushMappedMemoryRanges(
                    ranges.size(),
                    ranges.data()
                )
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

    ResizableBuffer::ResizableBuffer() {}

    ResizableBuffer::ResizableBuffer(
        const vk::BufferUsageFlags& usage,
        u64 size,
        const vk::MemoryPropertyFlags& flags
    ) : bufferFlags(usage), memoryFlags(flags) {
        Buffer::size = size;
    }

    ResizableBuffer::ResizableBuffer(
        Renderer& renderer,
        const vk::BufferUsageFlags& usage,
        u64 size,
        bool allocate,
        const vk::MemoryPropertyFlags& flags
    ) : bufferFlags(usage), memoryFlags(flags) {
        Buffer::size = size;
        Buffer::offset = 0;
        createBuffer(renderer);
        if (allocate) {
            allocateMemory(renderer.getRenderingDevice(), flags);
        }
    }

    void ResizableBuffer::createBuffer(const Renderer& renderer) {
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