#ifndef UNNECESSARYENGINE_PHONG_RENDERING_PIPELINE_H
#define UNNECESSARYENGINE_PHONG_RENDERING_PIPELINE_H

#include <unnecessary/rendering/rendering_pipeline.h>
#include <unnecessary/rendering/passes/draw_objects_pass.h>
#include <phong.gen.h>

namespace un {
    class PhongRenderingPipeline : public un::RenderingPipeline {
    protected:
        un::RenderGroup opaqueGroup;

    public:
        void configure(RenderGraph& graph) override {
            vk::Rect2D attachmentSize;
            graph.enqueuePass<un::DrawObjectsPass>(
                &opaqueGroup,
                attachmentSize,
                std::vector<vk::ClearValue>(
                    {
                        vk::ClearColorValue(std::array<float, 4>({0, 0, 0, 1}))
                    }
                )
            );

        }
    };
}
#endif
