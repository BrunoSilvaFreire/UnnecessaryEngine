#include <unnecessary/graphics/rendering/rendering_pipeline.h>
#include <unnecessary/graphics/renderer.h>
#include "unnecessary/graphics/rendering/frame_resource.h"

namespace un {
    void RenderingPipeline::begin(un::Renderer* renderer, vk::Framebuffer framebuffer) {
        for (auto[pass, index]: frameGraph->getFrameGraph().all_vertices()) {
            vk::CommandBufferInheritanceInfo inheritanceInfo(
                frameGraph->getVulkanPass(),
                index,
                framebuffer
            );

            frameGraph->getRenderPass(index).begin(&inheritanceInfo);
        }
    }

    un::BakedFrameGraph& RenderingPipeline::unsafeGetFrameGraph() {
        if (!frameGraph.has_value()) {
            throw std::runtime_error("Rendering pipeline has not been initialized.");
        }
        return *frameGraph;
    }

    void RenderingPipeline::storePassInto(
        const RenderPassDescriptor& descriptor,
        un::RenderPass** into
    ) {
        imports.emplace_back(std::make_pair(descriptor.getPassIndex(), into));
    }

    RenderingPipeline::~RenderingPipeline() {
        frameGraph.reset();
    }

    void RenderingPipeline::bake(un::Renderer* renderer) {
        un::FrameGraph graph;
        configureGraph(graph);
        frameGraph = un::BakedFrameGraph(std::move(graph), *renderer);
        for (auto[index, storage]: imports) {
            *storage = &frameGraph->getRenderPass(index);
        }
    }

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