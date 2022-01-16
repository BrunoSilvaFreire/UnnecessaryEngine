#ifndef UNNECESSARYENGINE_RENDER_JOB_H
#define UNNECESSARYENGINE_RENDER_JOB_H

#include <unnecessary/rendering/jobs/graphics_worker.h>
#include <unnecessary/rendering/rendering_pipeline.h>
#include <vector>

namespace un {
    struct PassOutput {
        vk::Semaphore doneSemaphore;
        vk::PipelineStageFlags pipelineStage;
        vk::CommandBuffer passCommands;
    };
    /**
     * Contains information needed for rendering a single frame
     */
    struct FrameData {
    public:
        std::vector<un::PassOutput> passesOutputs;

    };

    class RenderPassJob : public un::GraphicsJob {
    private:
        const un::PassData* _data;
        const un::RenderPass* _pass;
    public:


        RenderPassJob(
            const un::PassData* data,
            const un::RenderPass* pass
        ) : _data(data),
            _pass(pass) {}

        void operator()(un::GraphicsWorker* worker) override {
            un::CommandBuffer buf = worker->requestCommandBuffer();
            _pass->record(*_data, buf);
        }
    };
}
#endif
