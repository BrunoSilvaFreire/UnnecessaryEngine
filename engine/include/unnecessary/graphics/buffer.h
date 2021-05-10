//
// Created by Bruno on 10/5/2020.
//

#ifndef UNNECESSARYENGINE_BUFFER_H
#define UNNECESSARYENGINE_BUFFER_H

#include <unnecessary/graphics/renderer.h>

namespace un {
    class Buffer {
    private:
        vk::Buffer buffer;
        vk::DeviceMemory memory;
    public:
        Buffer() = default;

        Buffer(un::Renderer &renderer, vk::BufferUsageFlags usage, u64 size, bool allocate);

        const vk::Buffer &getVulkanBuffer() const;

        const vk::DeviceMemory &getMemory() const;

        operator vk::Buffer &();

        operator vk::Buffer() const;
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

        virtual ~CommandBuffer();

        vk::CommandBuffer getVulkanBuffer();
    };
}
#endif //UNNECESSARYENGINE_BUFFER_H
