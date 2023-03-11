#ifndef UNNECESSARYENGINE_FRAMEBUFFER_H
#define UNNECESSARYENGINE_FRAMEBUFFER_H

#include <vulkan/vulkan.hpp>
#include <unnecessary/rendering/disposable.h>
#include <unnecessary/rendering/image.h>
#include <unnecessary/rendering/image_view.h>
#include <unnecessary/rendering/vulkan/vulkan_wrapper.h>

namespace un {
    class renderer;

    class render_graph;

    class frame_buffer : public vulkan_wrapper<vk::Framebuffer> {
    private:
        std::vector<image> _ownedImages;
        std::vector<image_view> _ownedImagesView;

    public:
        frame_buffer(
            renderer* renderer,
            const render_graph& graph,
            vk::RenderPass renderPass,
            std::size_t frameBufferIndex
        );
    };
}
#endif
