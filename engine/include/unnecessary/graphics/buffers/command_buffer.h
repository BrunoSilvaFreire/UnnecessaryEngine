//
// Created by bruno on 09/08/2021.
//

#ifndef UNNECESSARYENGINE_COMMAND_BUFFER_H
#define UNNECESSARYENGINE_COMMAND_BUFFER_H

#include <unnecessary/graphics/renderer.h>

namespace un {

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

        vk::CommandBuffer* operator->();

        vk::CommandBuffer& operator*();
    };

}

#endif //UNNECESSARYENGINE_COMMAND_BUFFER_H
