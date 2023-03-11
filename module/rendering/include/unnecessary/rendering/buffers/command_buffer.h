//
// Created by bruno on 09/08/2021.
//

#ifndef UNNECESSARYENGINE_COMMAND_BUFFER_H
#define UNNECESSARYENGINE_COMMAND_BUFFER_H

#include <vulkan/vulkan.hpp>
#include <unnecessary/def.h>

namespace un {
    class renderer;

    class command_buffer {
    private:
        vk::CommandBuffer _buffer;
        vk::CommandPool _pool;
        vk::Device _device;

    public:
        command_buffer(
            renderer& renderer,
            vk::CommandPool pool,
            vk::CommandBufferLevel level = vk::CommandBufferLevel::ePrimary
        );

        virtual ~command_buffer();

        vk::CommandBuffer get_vulkan_buffer();

        const vk::CommandBuffer* operator->() const;

        vk::CommandBuffer& operator*();

        const vk::CommandBuffer& operator*() const;
    };
}

#endif //UNNECESSARYENGINE_COMMAND_BUFFER_H
