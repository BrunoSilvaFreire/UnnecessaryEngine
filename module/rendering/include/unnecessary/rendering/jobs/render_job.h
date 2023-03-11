#ifndef UNNECESSARYENGINE_RENDER_JOB_H
#define UNNECESSARYENGINE_RENDER_JOB_H

#include <unnecessary/rendering/jobs/graphics_worker.h>
#include <unnecessary/rendering/rendering_pipeline.h>
#include <vector>

namespace un {
    class record_pass_job : public graphics_job {
    private:
        frame_data* _data;
        const std::size_t _passIndex;
        const render_pass* _pass;

    public:
        record_pass_job(
            frame_data* data,
            const render_pass* pass,
            const std::size_t passIndex
        ) : _data(data),
            _passIndex(passIndex),
            _pass(pass) {
        }

        void operator()(graphics_worker* worker) override {
            command_buffer buf = _data->request_command_buffer(worker, _passIndex);
            const auto& inheritanceInfo = vk::CommandBufferInheritanceInfo(
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
         void operator()(worker_type* worker) override {
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

             vk::Queue queue = *_renderer->getDevice().get_graphics();
             queue.submit(
                 submits,
                 synchronizer.fence
             );
         }
     };*/
}
#endif
