#ifndef UNNECESSARYENGINE_PHONG_RENDERING_PIPELINE_H
#define UNNECESSARYENGINE_PHONG_RENDERING_PIPELINE_H

#include <unnecessary/rendering/rendering_pipeline.h>
#include <unnecessary/rendering/renderer.h>
#include <unnecessary/rendering/passes/draw_objects_pass.h>
#include <phong.gen.h>

namespace un {
    class phong_rendering_pipeline : public rendering_pipeline {
    protected:
        render_group _opaqueGroup;

    public:
        void configure(renderer& renderer, render_graph& graph) override {
            size_t color = graph.add_borrowed_attachment(
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
            graph.set_attachment_name(color, "Color");
            size_t depth = graph.add_owned_attachment(
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
            graph.set_attachment_name(depth, "DepthBuffer");
            vk::Rect2D attachmentSize{};
            const auto& resolution = renderer.get_swap_chain().get_resolution();
            attachmentSize.extent.width = resolution.x;
            attachmentSize.extent.height = resolution.y;
            graph.enqueue_pass<draw_objects_pass>(
                &_opaqueGroup,
                attachmentSize,
                std::vector<vk::ClearValue>(
                    {
                        graph.get_attachment(color).get_clear(),
                        graph.get_attachment(depth).get_clear()
                    }
                ),
                color,
                depth
            );
        }
    };
}
#endif
