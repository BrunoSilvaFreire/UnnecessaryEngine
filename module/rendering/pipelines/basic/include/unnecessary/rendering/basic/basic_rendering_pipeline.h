#ifndef UNNECESSARYENGINE_BASIC_RENDERING_PIPELINE_H
#define UNNECESSARYENGINE_BASIC_RENDERING_PIPELINE_H

#include <unnecessary/rendering/rendering_pipeline.h>
#include <unnecessary/rendering/renderer.h>
#include <unnecessary/rendering/passes/draw_objects_pass.h>

namespace un {
    class basic_rendering_pipeline : public rendering_pipeline {
    protected:
        render_group _opaqueGroup;
        std::size_t _color, _depth;

    public:
        void configure(renderer& renderer, render_graph& graph) override {
            _color = graph.add_borrowed_attachment(
                vk::ClearColorValue(
                    std::array<float, 4>({0, 1, 0, 1})
                ),
                static_cast<vk::AttachmentDescriptionFlags>(0),
                vk::Format::eB8G8R8A8Unorm,
                vk::SampleCountFlagBits::e1,
                vk::AttachmentLoadOp::eClear,
                vk::AttachmentStoreOp::eStore,
                vk::AttachmentLoadOp::eDontCare,
                vk::AttachmentStoreOp::eDontCare,
                vk::ImageLayout::eUndefined,
                vk::ImageLayout::ePresentSrcKHR
            );
            graph.set_attachment_name(_color, "Color");
            _depth = graph.add_owned_attachment(
                vk::ImageUsageFlagBits::eDepthStencilAttachment,
                vk::ImageAspectFlagBits::eDepth,
                vk::ClearDepthStencilValue(0, 0),
                static_cast<vk::AttachmentDescriptionFlags>(0),
                vk::Format::eD32Sfloat,
                vk::SampleCountFlagBits::e1,
                vk::AttachmentLoadOp::eClear,
                vk::AttachmentStoreOp::eStore,
                vk::AttachmentLoadOp::eClear,
                vk::AttachmentStoreOp::eStore,
                vk::ImageLayout::eUndefined,
                vk::ImageLayout::eDepthStencilAttachmentOptimal
            );
            graph.set_attachment_name(_depth, "Depth");
            vk::Rect2D attachmentSize{};
            const auto& resolution = renderer.get_swap_chain().get_resolution();
            attachmentSize.extent.width = resolution.x;
            attachmentSize.extent.height = resolution.y;
        }

        size_t get_color() const {
            return _color;
        }

        size_t get_depth() const {
            return _depth;
        }
    };
}
#endif
