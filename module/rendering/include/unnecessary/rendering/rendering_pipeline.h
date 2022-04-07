//
// Created by bruno on 20/12/2021.
//

#ifndef UNNECESSARYENGINE_RENDERING_PIPELINE_H
#define UNNECESSARYENGINE_RENDERING_PIPELINE_H

#include <unnecessary/rendering/render_pass.h>

namespace un {

    class RenderingPipeline {
    private:
    public:
        virtual void configure(un::Renderer& renderer, RenderGraph& graph) = 0;
    };
}
#endif
