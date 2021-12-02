#ifndef UNNECESSARYENGINE_RENDERING_PIPELINE_H
#define UNNECESSARYENGINE_RENDERING_PIPELINE_H

#include <unnecessary/graphics/rendering/baked_frame_graph.h>
#include <unnecessary/jobs/jobs.h>
#include <optional>
#include "frame_resource.h"

namespace un {
    class RenderingPipeline {
    private:
        std::optional<un::BakedFrameGraph> frameGraph;
        std::vector<std::pair<u32, un::RenderPass**>> imports;
    protected:
        virtual void configureGraph(un::FrameGraph& graph) = 0;

        void storePassInto(
            const un::RenderPassDescriptor& descriptor,
            un::RenderPass** into
        );
    public:
        virtual ~RenderingPipeline();

        void bake(un::Renderer* renderer);

        void begin(un::Renderer* renderer, vk::Framebuffer framebuffer);

        BakedFrameGraph& unsafeGetFrameGraph();
    };



}
#endif