#include <unnecessary/graphics/rendering/rendering_pipeline.h>
#include <unnecessary/graphics/renderer.h>

namespace un {
    void RenderingPipeline::begin(un::Renderer* renderer, vk::Framebuffer framebuffer) {
        for (auto[pass, index] : frameGraph->getFrameGraph().all_vertices()) {
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
        for (auto[index, storage] : imports) {
            *storage = &frameGraph->getRenderPass(index);
        }
    }

    un::RenderPass* DummyRenderingPipeline::getDepthPass() const {
        return depthPass;
    }

    un::RenderPass* DummyRenderingPipeline::getDrawOpaquePass() const {
        return drawOpaquePass;
    }

    un::FrameResource* DummyRenderingPipeline::getDepthBuffer() const {
        return depthBuffer;
    }

    void DummyRenderingPipeline::configureGraph(FrameGraph& graph) {
        colorAttachmentHandle = graph.addAttachment(
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
        auto drawOpaqueInfo = graph.addPass(
            "drawOpaquePass",
            vk::PipelineStageFlagBits::eFragmentShader |
            vk::PipelineStageFlagBits::eVertexShader |
            vk::PipelineStageFlagBits::eComputeShader
        );
        drawOpaqueInfo.usesColorAttachment(
            colorAttachmentHandle,
            vk::ImageLayout::eColorAttachmentOptimal
        );
        storePassInto(drawOpaqueInfo, &drawOpaquePass);
    }
}