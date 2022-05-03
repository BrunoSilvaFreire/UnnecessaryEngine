#ifndef UNNECESSARYENGINE_basic_RENDERING_PIPELINE_H
#define UNNECESSARYENGINE_basic_RENDERING_PIPELINE_H

#include <unnecessary/rendering/rendering_pipeline.h>
#include <unnecessary/rendering/renderer.h>
#include <unnecessary/rendering/passes/draw_objects_pass.h>

namespace un {
    class BasicRenderingPipeline : public un::RenderingPipeline {
    protected:
        un::RenderGroup opaqueGroup;
        std::size_t color, depth;
    public:
        void configure(un::Renderer& renderer, RenderGraph& graph) override {
            color = graph.addBorrowedAttachment(
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
            graph.setAttachmentName(color, "Color");
            depth = graph.addOwnedAttachment(
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
            graph.setAttachmentName(depth, "Depth");
            vk::Rect2D attachmentSize{};
            const auto& resolution = renderer.getSwapChain().getResolution();
            attachmentSize.extent.width = resolution.x;
            attachmentSize.extent.height = resolution.y;
        }

        size_t getColor() const {
            return color;
        }

        size_t getDepth() const {
            return depth;
        }
    };
}
#endif
