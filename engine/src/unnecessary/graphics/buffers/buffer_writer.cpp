#include "unnecessary/graphics/buffers/buffer_writer.h"

namespace un {
    BufferWriter::~BufferWriter() {
        cmd->end();
        std::array<vk::CommandBuffer, 1> arr{*cmd};
        renderer->getGraphics()->submit({vk::SubmitInfo({}, {}, arr)});
    }

    BufferWriter::BufferWriter(
        un::Renderer* renderer,
        vk::CommandPool pool
    ) : renderer(renderer),
        cmd(
            *renderer,
            pool
            ? pool
            : renderer
                ->getGlobalPool(),
            vk::CommandBufferUsageFlagBits::eOneTimeSubmit
        ) {
        cmd->begin(
            vk::CommandBufferBeginInfo(
                vk::CommandBufferUsageFlagBits::eOneTimeSubmit
            )
        );
    }

    void BufferWriter::overwriteWithStaging(const Buffer& buffer, void* ptr) {
        vk::DeviceSize size = buffer.getSize();
        un::Buffer stagingBuffer(
            *renderer,
            vk::BufferUsageFlagBits::eTransferSrc,
            size,
            true,
            vk::MemoryPropertyFlagBits::eHostVisible
        );
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

    void BufferWriter::overwrite(Buffer& buffer, void* ptr) {
        buffer.push(renderer->getVirtualDevice(), ptr);
    }

}