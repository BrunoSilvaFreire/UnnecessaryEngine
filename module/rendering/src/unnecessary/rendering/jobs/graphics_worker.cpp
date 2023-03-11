#include <unnecessary/rendering/jobs/graphics_worker.h>
#include <unnecessary/rendering/renderer.h>

namespace un {
    command_buffer graphics_worker::request_command_buffer() const {
        return command_buffer(
            *_renderer,
            _commandPool,
            vk::CommandBufferLevel::ePrimary
        );
    }

    graphics_worker::graphics_worker(
        renderer* renderer,
        size_t index,
        bool autostart
    ) : job_worker_mixin(index, autostart), _renderer(renderer) {
        vk::Device device = renderer->get_virtual_device();
        _commandPool = device.createCommandPool(
            vk::CommandPoolCreateInfo(
                vk::CommandPoolCreateFlagBits::eTransient,
                renderer->get_device().get_graphics().get_index()
            )
        );
        std::ostringstream name;
        name << "GraphicsWorkerCommandPool-";
        name << index;
        renderer->tag(_commandPool, name.str());
    }

    renderer* graphics_worker::get_renderer() const {
        return _renderer;
    }
}
