#ifndef UNNECESSARYENGINE_FRAMEBUFFER_H
#define UNNECESSARYENGINE_FRAMEBUFFER_H

#include <vulkan/vulkan.hpp>
#include <unnecessary/graphics/image.h>
#include <unnecessary/graphics/image_view.h>
#include <unnecessary/graphics/rendering/frame_graph.h>

namespace un {
    class BakedFrameGraph;

    class Renderer;

    class FrameBuffer {
    private:
        vk::Framebuffer framebuffer;
        std::vector<un::Image> ownedImages;
        std::vector<un::ImageView> ownedImagesView;
    public:
        FrameBuffer(
            un::Renderer* renderer,
            const un::FrameGraph& graph,
            vk::RenderPass renderPass,
            std::size_t frameBufferIndex
        );

        operator vk::Framebuffer();
    };

}
#endif
