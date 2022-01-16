#include <unnecessary/rendering/jobs/graphics_worker.h>

namespace un {
    un::CommandBuffer GraphicsWorker::requestCommandBuffer() const {
        return un::CommandBuffer(
            *_renderer,
            _commandPool,
            vk::CommandBufferLevel::eSecondary
        );
    }

    GraphicsWorker::GraphicsWorker(
        un::Renderer* renderer,
        size_t index,
        bool autostart,
        const JobProvider <JobType>& provider,
        const JobNotifier <JobType>& notifier
    ) : AbstractJobWorker(index, autostart, provider, notifier), _renderer(renderer) {}
}