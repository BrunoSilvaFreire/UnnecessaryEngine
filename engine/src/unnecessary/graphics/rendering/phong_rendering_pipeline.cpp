#include <unnecessary/graphics/rendering/phong_rendering_pipeline.h>
namespace un {

    un::RenderPass* PhongRenderingPipeline::getDepthPass() const {
        return depthPass;
    }

    un::RenderPass* PhongRenderingPipeline::getDrawOpaquePass() const {
        return drawOpaquePass;
    }

    FrameResource* PhongRenderingPipeline::getDepthBuffer() const {
        return depthBuffer;
    }

    void PhongRenderingPipeline::configureGraph(FrameGraph& graph) {
        colorAttachmentHandle = graph.addBorrowedAttachment(
            vk::ClearColorValue(
                std::array<float, 4>(
                    {
                        0.1F, 0.1F, 0.1F, 1.0F
                    }
                )
            ),
            (vk::AttachmentDescriptionFlags) 0,
            vk::Format::eB8G8R8A8Unorm,
            vk::SampleCountFlagBits::e1,
            vk::AttachmentLoadOp::eClear,
            vk::AttachmentStoreOp::eStore,
            vk::AttachmentLoadOp::eDontCare,
            vk::AttachmentStoreOp::eDontCare,
            vk::ImageLayout::eUndefined,
            vk::ImageLayout::ePresentSrcKHR
        );
        depthAttachmentHandle = graph.addOwnedAttachment(
            vk::ImageUsageFlagBits::eDepthStencilAttachment,
            vk::ImageAspectFlagBits::eDepth,
            vk::ClearDepthStencilValue(1, 0),
            (vk::AttachmentDescriptionFlags) 0,
            vk::Format::eD16Unorm,
            vk::SampleCountFlagBits::e1,
            vk::AttachmentLoadOp::eClear,
            vk::AttachmentStoreOp::eStore,
            vk::AttachmentLoadOp::eClear,
            vk::AttachmentStoreOp::eStore,
            vk::ImageLayout::eUndefined,
            vk::ImageLayout::eDepthStencilAttachmentOptimal
        );
        auto drawOpaque = graph.addPass(
            "drawOpaquePass",
            vk::PipelineStageFlagBits::eFragmentShader |
            vk::PipelineStageFlagBits::eVertexShader
        );
        drawOpaque.usesColorAttachment(
            colorAttachmentHandle,
            vk::ImageLayout::eColorAttachmentOptimal
        );
        drawOpaque.usesDepthAttachment(
            depthAttachmentHandle,
            vk::ImageLayout::eDepthStencilAttachmentOptimal
        );
        storePassInto(drawOpaque, &drawOpaquePass);
    }
}