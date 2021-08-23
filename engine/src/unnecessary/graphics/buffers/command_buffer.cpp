#include <unnecessary/graphics/buffers/command_buffer.h>
#include <unnecessary/graphics/renderer.h>

namespace un {

    CommandBuffer::CommandBuffer(Renderer& renderer) : CommandBuffer(
        renderer,
        renderer.getGlobalPool()) {

    }

    CommandBuffer::CommandBuffer(
        Renderer& renderer,
        vk::CommandPool pool
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

    const vk::CommandBuffer* CommandBuffer::operator->() const {
        return &buffer;
    }

    vk::CommandBuffer& CommandBuffer::operator*() {
        return buffer;
    }

    const vk::CommandBuffer& CommandBuffer::operator*() const {
        return buffer;
    }

}