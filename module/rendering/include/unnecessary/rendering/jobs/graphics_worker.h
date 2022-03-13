#ifndef UNNECESSARYENGINE_GRAPHICS_WORKER_H
#define UNNECESSARYENGINE_GRAPHICS_WORKER_H

#include <vulkan/vulkan.hpp>
#include <unnecessary/jobs/workers/worker.h>
#include <unnecessary/rendering/buffers/command_buffer.h>
#include <unnecessary/jobs/worker_chain.h>

namespace un {
    class GraphicsWorker;

    typedef un::Job<GraphicsWorker> GraphicsJob;

    class GraphicsWorker : public un::AbstractJobWorker<un::GraphicsJob> {
    private:
        un::Renderer* _renderer;
        vk::CommandPool _commandPool;
    public:
        GraphicsWorker(
            un::Renderer* renderer,
            size_t index,
            bool autostart
        );

        un::CommandBuffer requestCommandBuffer() const;

        Renderer* getRenderer() const;
    };
    typedef un::WorkerChain<un::GraphicsWorker> GraphicsChain;


}
#endif
