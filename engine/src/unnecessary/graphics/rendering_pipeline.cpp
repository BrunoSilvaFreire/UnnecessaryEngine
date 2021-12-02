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

}