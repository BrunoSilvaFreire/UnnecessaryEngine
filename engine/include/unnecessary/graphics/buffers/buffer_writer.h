#ifndef UNNECESSARYENGINE_BUFFER_WRITER_H
#define UNNECESSARYENGINE_BUFFER_WRITER_H

#include <vulkan/vulkan.hpp>
#include <unnecessary/graphics/buffers/buffer.h>
#include <unnecessary/graphics/buffers/command_buffer.h>

namespace un {
    class BufferWriter {
    private:
        un::Renderer* renderer;
        vk::CommandPool pool;
        un::CommandBuffer cmd;
    public:
        BufferWriter(
            un::Renderer* renderer,
            vk::CommandPool pool = nullptr
        );

        ~BufferWriter();

        void overwriteWithStaging(const un::Buffer& buffer, void* ptr);

        void overwrite(un::Buffer& buffer, void* ptr);
    };
}


#endif //UNNECESSARYENGINE_BUFFER_WRITER_H
