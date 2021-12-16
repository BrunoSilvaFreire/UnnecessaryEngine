#ifndef UNNECESSARYENGINE_PHONG_RENDERING_PIPELINE_H
#define UNNECESSARYENGINE_PHONG_RENDERING_PIPELINE_H

#include <unnecessary/graphics/rendering/rendering_pipeline.h>

namespace un {
    class PhongRenderingPipeline : public un::RenderingPipeline {
    private:
        un::RenderPass* drawOpaquePass{};
        un::RenderPass* depthPass{};
        un::FrameResource* depthBuffer{};
        std::size_t colorAttachmentHandle{};
        std::size_t depthAttachmentHandle{};
    protected:
        void configureGraph(FrameGraph& graph) override;

    public:
        un::RenderPass* getDepthPass() const;

        un::RenderPass* getDrawOpaquePass() const;

        un::FrameResource* getDepthBuffer() const;
    };
}
#endif