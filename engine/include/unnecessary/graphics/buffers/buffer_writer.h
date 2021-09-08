#ifndef UNNECESSARYENGINE_BUFFER_WRITER_H
#define UNNECESSARYENGINE_BUFFER_WRITER_H

#include <vulkan/vulkan.hpp>
#include <unnecessary/graphics/buffers/buffer.h>
#include <unnecessary/graphics/buffers/command_buffer.h>
#include <unnecessary/graphics/dispose_tracker.h>

namespace un {
    class BufferWriter {
    private:
        un::Renderer* renderer;
        vk::CommandPool pool;
        un::CommandBuffer cmd;
        bool autoSubmit;
        un::DisposeTracker toDispose;
    public:
        BufferWriter(
            un::Renderer* renderer,
            vk::CommandPool pool = nullptr,
            bool autoSubmit = true
        );

        ~BufferWriter();

        void overwriteWithStaging(const un::Buffer& buffer, void* ptr);

        void overwrite(un::Buffer& buffer, void* ptr);

        const un::CommandBuffer& getCommandBuffer();
    };
}


#endif //UNNECESSARYENGINE_BUFFER_WRITER_H
