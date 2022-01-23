#include <unnecessary/rendering/jobs/graphics_worker.h>
#include <unnecessary/rendering/renderer.h>

namespace un {
    un::CommandBuffer GraphicsWorker::requestCommandBuffer() const {
        return un::CommandBuffer(
            *_renderer,
            _commandPool,
            vk::CommandBufferLevel::ePrimary
        );
    }

    GraphicsWorker::GraphicsWorker(
        un::Renderer* renderer,
        size_t index,
        bool autostart,
        const JobProvider<JobType>& provider,
        const JobNotifier<JobType>& notifier
    ) : AbstractJobWorker(index, autostart, provider, notifier), _renderer(renderer) {

        vk::Device device = renderer->getVirtualDevice();
        _commandPool = device.createCommandPool(
            vk::CommandPoolCreateInfo(
                vk::CommandPoolCreateFlagBits::eTransient,
                renderer->getDevice().getGraphics().getIndex()
            )
        );
    }
}