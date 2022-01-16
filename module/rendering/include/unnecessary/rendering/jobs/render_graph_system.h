#ifndef UNNECESSARYENGINE_RENDER_GRAPH_SYSTEM_H
#define UNNECESSARYENGINE_RENDER_GRAPH_SYSTEM_H

#include <unnecessary/rendering/render_pass.h>
#include <unnecessary/rendering/renderer.h>
#include <unnecessary/rendering/render_graph.h>
#include <unnecessary/rendering/jobs/graphics_worker.h>
#include <unnecessary/rendering/jobs/render_job.h>
#include <unnecessary/simulation/systems/system.h>

namespace un {


    template<typename JobSystemType>
    class RenderGraphSystem : public un::System {
    private:
        un::RenderGraph* _graph;
        un::Renderer* _renderer;


    public:

        void scheduleJobs(
            World& world,
            float deltaTime,
            SimulationChain& chain
        ) override {

        }
    };
}
#endif
