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
        bool autostart
    ) : AbstractJobWorker(index, autostart), _renderer(renderer) {

        vk::Device device = renderer->getVirtualDevice();
        _commandPool = device.createCommandPool(
            vk::CommandPoolCreateInfo(
                vk::CommandPoolCreateFlagBits::eTransient,
                renderer->getDevice().getGraphics().getIndex()
            )
        );
        std::ostringstream name;
        name << "GraphicsWorkerCommandPool-";
        name << index;
        renderer->tag(_commandPool, name.str());
    }

    Renderer* GraphicsWorker::getRenderer() const {
        return _renderer;
    }
}