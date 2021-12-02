#include "unnecessary/graphics/buffers/buffer_writer.h"

namespace un {
    BufferWriter::~BufferWriter() {
        cmd->end();
        if (autoSubmit) {
            std::array<vk::CommandBuffer, 1> arr{*cmd};
            renderer->getGraphics()->submit({vk::SubmitInfo({}, {}, arr)});
            toDispose.dispose(renderer->getVirtualDevice());
        }
    }

    BufferWriter::BufferWriter(
        un::Renderer* renderer,
        vk::CommandPool pool,
        bool autoSubmit
    ) : renderer(renderer),
        cmd(
            *renderer,
            pool ? pool : renderer->getGlobalPool()
        ), autoSubmit(autoSubmit) {
        cmd->begin(
            vk::CommandBufferBeginInfo(
                vk::CommandBufferUsageFlagBits::eOneTimeSubmit
            )
        );
    }

    void BufferWriter::overwriteWithStaging(const GPUBuffer& buffer, void* ptr) {
        vk::DeviceSize size = buffer.getSize();
        un::GPUBuffer stagingBuffer(
            *renderer,
            vk::BufferUsageFlagBits::eTransferSrc,
            size,
            true,
            vk::MemoryPropertyFlagBits::eHostVisible
        );
        toDispose.include(stagingBuffer);
        toDispose.include(stagingBuffer.getMemory());
        stagingBuffer.push(renderer->getVirtualDevice(), ptr);
        cmd->copyBuffer(
            stagingBuffer, buffer,
            {
                vk::BufferCopy(
                    0,
                    0,
                    size
                )
            }
        );
    }

    void BufferWriter::overwrite(GPUBuffer& buffer, void* ptr) {
        buffer.push(renderer->getVirtualDevice(), ptr);
    }

    const un::CommandBuffer& BufferWriter::getCommandBuffer() {
        return cmd;
    }

}