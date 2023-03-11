#ifndef UNNECESSARYENGINE_GRAPHICS_WORKER_H
#define UNNECESSARYENGINE_GRAPHICS_WORKER_H

#include <vulkan/vulkan.hpp>
#include <unnecessary/jobs/workers/worker.h>
#include <unnecessary/rendering/buffers/command_buffer.h>
#include <unnecessary/jobs/worker_chain.h>

namespace un {
    class graphics_worker;

    using graphics_job = job<graphics_worker>;

    class graphics_worker : public job_worker_mixin<graphics_job> {
    private:
        renderer* _renderer;
        vk::CommandPool _commandPool;

    public:
        graphics_worker(
            renderer* renderer,
            size_t index,
            bool autostart
        );

        command_buffer request_command_buffer() const;

        renderer* get_renderer() const;
    };

    using graphics_chain = worker_chain<graphics_worker>;
}
#endif
