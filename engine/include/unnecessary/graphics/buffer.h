//
// Created by Bruno on 10/5/2020.
//

#ifndef UNNECESSARYENGINE_BUFFER_H
#define UNNECESSARYENGINE_BUFFER_H

#include <unnecessary/graphics/renderer.h>

namespace un {
    class Buffer {
    protected:
        vk::Buffer buffer;
        vk::DeviceMemory memory;
        vk::DeviceSize offset;
        vk::DeviceSize size;
        vk::MemoryPropertyFlags memoryFlags;

        void allocateMemory(const un::RenderingDevice& renderingDevice, vk::MemoryPropertyFlags flags);

        vk::Buffer createBuffer() {

        }

    public:
        Buffer() = default;

        Buffer(
                un::Renderer& renderer,
                vk::BufferUsageFlags usage,
                u64 size,
                bool allocate,
                vk::MemoryPropertyFlags flags = static_cast<vk::MemoryPropertyFlags>(0)
        );

        const vk::Buffer& getVulkanBuffer() const;

        const vk::DeviceMemory& getMemory() const;

        operator vk::Buffer&();

        operator vk::Buffer() const;

        void push(vk::Device device, void* data, bool flush = true);

        vk::DeviceSize getOffset() const;

        vk::DeviceSize getSize() const;


    };

    class ResizableBuffer : public un::Buffer {
    private:
        vk::MemoryPropertyFlags memoryFlags;
    public:
        template<typename T>
        bool ensureFits(const un::RenderingDevice& device, size_t count, size_t extra = 0) {
            vk::DeviceSize required = (count * sizeof(T)) + extra;
            return ensureLargeEnough(device, required);
        }

        bool ensureLargeEnough(const un::RenderingDevice& device, vk::DeviceSize required) {
            const vk::Device& virtualDevice = device.getVirtualDevice();
            if (required > size) {
                virtualDevice.free(memory);
                size = required;
                allocateMemory(device, memoryFlags);
                return true;
            }
            return false;
        }
    };

    class CommandBuffer {
    private:
        vk::CommandBuffer buffer;
        vk::CommandPool pool;
        vk::Device device;
    public:
        CommandBuffer(
                un::Renderer& renderer
        );

        CommandBuffer(
                un::Renderer& renderer,
                vk::CommandPool pool
        );

        CommandBuffer(
                un::Renderer& renderer,
                vk::CommandPool pool,
                vk::CommandBufferUsageFlags usage
        );

        virtual ~CommandBuffer();

        vk::CommandBuffer getVulkanBuffer();
    };
}
#endif //UNNECESSARYENGINE_BUFFER_H
