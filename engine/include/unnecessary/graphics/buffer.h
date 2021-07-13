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

        void allocateMemory(const un::RenderingDevice &renderingDevice, vk::MemoryPropertyFlags flags);

    public:
        Buffer() = default;

        Buffer(
                un::Renderer &renderer,
                vk::BufferUsageFlags usage,
                u64 size,
                bool allocate,
                vk::MemoryPropertyFlags flags = static_cast<vk::MemoryPropertyFlags>(0)
        );

        const vk::Buffer &getVulkanBuffer() const;

        const vk::DeviceMemory &getMemory() const;

        operator vk::Buffer &();

        operator vk::Buffer() const;

        void push(vk::Device device, void *data, bool flush = true);

        vk::DeviceSize getOffset() const;

        vk::DeviceSize getSize() const;


    };

    class ResizableBuffer : public un::Buffer {
    private:
        ResizableBuffer(const vk::BufferUsageFlags &usage, u64 size, const vk::MemoryPropertyFlags &flags);

        vk::MemoryPropertyFlags memoryFlags;
        vk::BufferUsageFlags bufferFlags;
    public:
        ResizableBuffer();

        ResizableBuffer(
                un::Renderer &renderer,
                const vk::BufferUsageFlags &usage,
                u64 size,
                bool allocate,
                const vk::MemoryPropertyFlags &flags
        );

        template<typename T>
        bool ensureFits(Renderer &renderer, size_t count, size_t extra = 0) {
            vk::DeviceSize required = (count * sizeof(T)) + extra;
            return ensureLargeEnough(renderer, required);
        }

        bool ensureLargeEnough(Renderer &renderer, vk::DeviceSize required) {
            const vk::Device &virtualDevice = renderer.getVirtualDevice();
            if (required > size) {
                size = required;
                createBuffer(renderer);
                virtualDevice.free(memory);
                allocateMemory(renderer.getRenderingDevice(), memoryFlags);
                return true;
            }
            return false;
        }

        void createBuffer(const Renderer &renderer);
    };

    class CommandBuffer {
    private:
        vk::CommandBuffer buffer;
        vk::CommandPool pool;
        vk::Device device;
    public:
        CommandBuffer(
                un::Renderer &renderer
        );

        CommandBuffer(
                un::Renderer &renderer,
                vk::CommandPool pool
        );

        CommandBuffer(
                un::Renderer &renderer,
                vk::CommandPool pool,
                vk::CommandBufferUsageFlags usage
        );

        virtual ~CommandBuffer();

        vk::CommandBuffer getVulkanBuffer();
    };
}
#endif //UNNECESSARYENGINE_BUFFER_H
