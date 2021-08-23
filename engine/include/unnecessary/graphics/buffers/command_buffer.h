//
// Created by bruno on 09/08/2021.
//

#ifndef UNNECESSARYENGINE_COMMAND_BUFFER_H
#define UNNECESSARYENGINE_COMMAND_BUFFER_H

#include <vulkan/vulkan.hpp>

namespace un {
    class Renderer;

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

        virtual ~CommandBuffer();

        vk::CommandBuffer getVulkanBuffer();

        const vk::CommandBuffer* operator->() const;

        vk::CommandBuffer& operator*();

        const vk::CommandBuffer& operator*() const;
    };

}

#endif //UNNECESSARYENGINE_COMMAND_BUFFER_H
