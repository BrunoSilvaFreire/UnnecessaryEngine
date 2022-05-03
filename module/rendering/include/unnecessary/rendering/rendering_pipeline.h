//
// Created by bruno on 20/12/2021.
//

#ifndef UNNECESSARYENGINE_RENDERING_PIPELINE_H
#define UNNECESSARYENGINE_RENDERING_PIPELINE_H

#include <unnecessary/rendering/render_pass.h>
#include <memory>
#include <vector>

namespace un {

    class RenderingPipeline {
    private:
        std::vector<std::unique_ptr<un::RenderPass>> extraPasses;

    public:
        virtual void configure(un::Renderer& renderer, RenderGraph& graph) = 0;

        void setup(un::Renderer& renderer, RenderGraph& graph) {
            configure(renderer, graph);
            for (std::unique_ptr<un::RenderPass>& pass : extraPasses) {
                graph.enqueuePass(std::move(pass));
            }
            extraPasses.clear();
        };

        void enqueueExtraPass(std::unique_ptr<un::RenderPass>&& pass) {
            extraPasses.push_back(std::move(pass));
        }

    };
}
#endif
