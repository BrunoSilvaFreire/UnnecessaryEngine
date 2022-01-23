#ifndef UNNECESSARYENGINE_RENDER_JOB_H
#define UNNECESSARYENGINE_RENDER_JOB_H

#include <unnecessary/rendering/jobs/graphics_worker.h>
#include <unnecessary/rendering/rendering_pipeline.h>
#include <vector>

namespace un {


    class RecordPassJob : public un::GraphicsJob {
    private:
        un::FrameData* _data;
        const std::size_t _passIndex;
        const un::RenderPass* _pass;
    public:


        RecordPassJob(
            un::FrameData* data,
            const un::RenderPass* pass,
            const std::size_t passIndex
        ) : _data(data),
            _pass(pass),
            _passIndex(passIndex) {}

        void operator()(un::GraphicsWorker* worker) override {
            un::CommandBuffer buf = _data->requestCommandBuffer(worker, _passIndex);
            const vk::CommandBufferInheritanceInfo& inheritanceInfo = vk::CommandBufferInheritanceInfo(
                _data->renderPass,
                0,
                _data->framebuffer,
                false
            );
            buf->begin(
                vk::CommandBufferBeginInfo(
                    vk::CommandBufferUsageFlagBits::eOneTimeSubmit,
                    &inheritanceInfo
                )
            );
            _pass->record(*_data, buf);
            buf->end();
        }
    };

    /* class SubmitFrameJob : public un::GraphicsJob {
     private:
         un::FrameData* _data;
         un::Renderer* _renderer;
     public:
         void operator()(WorkerType* worker) override {
             std::vector<vk::SubmitInfo> submits;
             graph.each(
                 [&](u32 index, const un::RenderPass* const& pass) {
                     un::PassOutput& passOutput = frameData.passesOutputs[index];

                     std::vector<u32> dependencies = graph
                         .dependenciesOf(index)
                         .getDependencies();
                     std::vector<u32> dependants = graph
                         .dependantsOn(index)
                         .getDependencies();
                     std::vector<vk::Semaphore> waitSemaphores;
                     std::vector<vk::PipelineStageFlags> waitStageMask;
                     for (u32 dependency : dependencies) {
                         const auto& output = frameData.passesOutputs[dependency];
                         waitSemaphores.emplace_back(output.doneSemaphore);
                         waitStageMask.emplace_back(output.pipelineStage);
                     }
                     vk::SubmitInfo submit;
                     submit.setCommandBufferCount(1);
                     submit.setPCommandBuffers(&passOutput.passCommands);
                     if (!dependants.empty()) {
                         submit.setSignalSemaphoreCount(1);
                         submit.setPSignalSemaphores(&passOutput.doneSemaphore);
                     }
                     if (!waitSemaphores.empty()) {
                         submit.setWaitSemaphores(waitSemaphores);
                         submit.setWaitDstStageMask(waitStageMask);
                     }
                     submits.push_back(submit);
                 },
                 [&](u32 from, u32 to, const un::DependencyType& type) {}
             );

             vk::Queue queue = *_renderer->getDevice().getGraphics();
             queue.submit(
                 submits,
                 synchronizer.fence
             );
         }
     };*/
}
#endif
