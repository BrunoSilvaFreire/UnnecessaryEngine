#include <unnecessary/rendering/renderer.h>
#include "unnecessary/rendering/vulkan/vulkan_utils.h"
#include <unnecessary/rendering/buffers/command_buffer.h>

namespace un {
    command_buffer::command_buffer(
        renderer& renderer,
        vk::CommandPool pool,
        vk::CommandBufferLevel level
    ) : _pool(pool) {
        _device = renderer.get_virtual_device();
        vk::CommandBufferAllocateInfo allocInfo(
            pool,
            level,
            1
        );

        VK_CALL(
            _device.allocateCommandBuffers(
                &allocInfo,
                &_buffer
            )
        );
    }

    command_buffer::~command_buffer() {
        //usesPipeline.freeCommandBuffers(pool, 1, &buffer);
    }

    vk::CommandBuffer command_buffer::get_vulkan_buffer() {
        return _buffer;
    }

    const vk::CommandBuffer* command_buffer::operator->() const {
        return &_buffer;
    }

    vk::CommandBuffer& command_buffer::operator*() {
        return _buffer;
    }

    const vk::CommandBuffer& command_buffer::operator*() const {
        return _buffer;
    }
}
