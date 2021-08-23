#include <unnecessary/graphics/rendering/rendering_pipeline.h>
#include <unnecessary/graphics/renderer.h>

namespace un {
    void RenderingPipeline::begin(vk::Framebuffer framebuffer) {
        auto resolution = renderer->getSwapChain().getResolution();
        vk::Rect2D rect(
            vk::Offset2D(0, 0),
            vk::Extent2D(resolution.x, resolution.y)
        );

        for (auto[pass, index] : frameGraph->getFrameGraph().all_vertices()) {
            vk::CommandBufferInheritanceInfo inheritanceInfo;
            inheritanceInfo.setSubpass(index);
            configureInheritance(inheritanceInfo);
            pass->begin(
                &inheritanceInfo,
                *frameGraph,
                rect,
                framebuffer
            );
        }
    }

    BakedFrameGraph* RenderingPipeline::getFrameGraph() const {
        return frameGraph;
    }
}