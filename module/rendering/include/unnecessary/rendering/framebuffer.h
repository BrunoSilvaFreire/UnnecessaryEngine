#ifndef UNNECESSARYENGINE_FRAMEBUFFER_H
#define UNNECESSARYENGINE_FRAMEBUFFER_H

#include <vulkan/vulkan.hpp>
#include <unnecessary/rendering/disposable.h>
#include <unnecessary/rendering/image.h>
#include <unnecessary/rendering/image_view.h>
#include <unnecessary/rendering/vulkan/vulkan_wrapper.h>

namespace un {

    class Renderer;

    class RenderGraph;

    class FrameBuffer : public VulkanWrapper<vk::Framebuffer> {
    private:
        std::vector<un::Image> ownedImages;
        std::vector<un::ImageView> ownedImagesView;
    public:
        FrameBuffer(
            un::Renderer* renderer,
            const un::RenderGraph& graph,
            vk::RenderPass renderPass,
            std::size_t frameBufferIndex
        );

    };

}
#endif
