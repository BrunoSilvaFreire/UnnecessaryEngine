#ifndef UNNECESSARYENGINE_BAKED_FRAME_GRAPH_H
#define UNNECESSARYENGINE_BAKED_FRAME_GRAPH_H

#include <unnecessary/graphics/rendering/renderpass.h>
#include <unnecessary/graphics/rendering/frame_graph.h>
#include <unnecessary/graphics/framebuffer.h>

namespace un {

    class BakedFrameGraph {
    private:
        FrameGraph frameGraph;
        vk::RenderPass renderPass;
        std::vector<un::RenderPass> bakedPasses;
        std::vector<un::FrameBuffer> frameBuffers;
    public:
        BakedFrameGraph(un::FrameGraph&& graph, un::Renderer& renderer);

        const vk::RenderPass& getVulkanPass() const;

        un::FrameGraph& getFrameGraph();

        const un::FrameGraph& getFrameGraph() const;

        un::RenderPass& getRenderPass(u32 passIndex);

        un::RenderPass& getRenderPass(const un::RenderPassDescriptor& descriptor);

        un::FrameBuffer& getFrameBuffer(u32 i);
    };
}
#endif
