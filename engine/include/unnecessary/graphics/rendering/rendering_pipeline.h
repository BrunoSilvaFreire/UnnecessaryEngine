#ifndef UNNECESSARYENGINE_RENDERING_PIPELINE_H
#define UNNECESSARYENGINE_RENDERING_PIPELINE_H

#include <unnecessary/graphics/frame_graph.h>
#include <unnecessary/jobs/jobs.h>
#include <optional>

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


    class DummyRenderingPipeline : public un::RenderingPipeline {
    private:
        un::RenderPass* drawOpaquePass{};
        un::RenderPass* depthPass{};
        un::FrameResource* depthBuffer{};
        std::size_t colorAttachmentHandle{};
    protected:
        void configureGraph(FrameGraph& graph) override;

    public:
        un::RenderPass* getDepthPass() const;

        un::RenderPass* getDrawOpaquePass() const;

        un::FrameResource* getDepthBuffer() const;
    };
}
#endif