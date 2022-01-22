#ifndef UNNECESSARYENGINE_RENDER_JOB_H
#define UNNECESSARYENGINE_RENDER_JOB_H

#include <unnecessary/rendering/jobs/graphics_worker.h>
#include <unnecessary/rendering/rendering_pipeline.h>
#include <vector>

namespace un {



    class RenderPassJob : public un::GraphicsJob {
    private:
        const un::FrameData* _data;
        const un::RenderPass* _pass;
    public:


        RenderPassJob(
            const un::FrameData* data,
            const un::RenderPass* pass
        ) : _data(data),
            _pass(pass) {}

        void operator()(un::GraphicsWorker* worker) override {
            un::CommandBuffer buf = worker->requestCommandBuffer();
            const vk::CommandBufferInheritanceInfo& inheritanceInfo = vk::CommandBufferInheritanceInfo(
                _data->renderPass,
                0,
                _data->framebuffer,
                false
            );
            buf->begin(
                vk::CommandBufferBeginInfo(
                    vk::CommandBufferUsageFlagBits::eRenderPassContinue |
                    vk::CommandBufferUsageFlagBits::eOneTimeSubmit,
                    &inheritanceInfo
                )
            );
            _pass->record(*_data, buf);
            buf->end();
        }
    };
}
#endif
