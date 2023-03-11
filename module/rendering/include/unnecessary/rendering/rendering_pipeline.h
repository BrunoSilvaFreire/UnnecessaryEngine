//
// Created by bruno on 20/12/2021.
//

#ifndef UNNECESSARYENGINE_RENDERING_PIPELINE_H
#define UNNECESSARYENGINE_RENDERING_PIPELINE_H

#include <unnecessary/rendering/render_pass.h>
#include <memory>
#include <vector>

namespace un {

    class rendering_pipeline {
    private:
        std::vector<std::unique_ptr<un::render_pass>> _extraPasses;

    public:
        virtual void configure(un::renderer& renderer, render_graph& graph) = 0;

        void setup(un::renderer& renderer, render_graph& graph) {
            configure(renderer, graph);
            for (std::unique_ptr<un::render_pass>& pass : _extraPasses) {
                graph.enqueue_pass(std::move(pass));
            }
            _extraPasses.clear();
        };

        void enqueue_extra_pass(std::unique_ptr<un::render_pass>&& pass) {
            _extraPasses.push_back(std::move(pass));
        }

    };
}
#endif
