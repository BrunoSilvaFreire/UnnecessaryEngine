#ifndef UNNECESSARYENGINE_RENDER_GRAPH_SYSTEM_H
#define UNNECESSARYENGINE_RENDER_GRAPH_SYSTEM_H

#include <unnecessary/rendering/render_pass.h>
#include <unnecessary/rendering/renderer.h>
#include <unnecessary/rendering/render_graph.h>
#include <unnecessary/rendering/jobs/graphics_worker.h>
#include <unnecessary/rendering/jobs/render_job.h>
#include <unnecessary/simulation/systems/system.h>

namespace un {
    template<typename t_job_system>
    class render_graph_system : public system {
    private:
        render_graph* _graph;
        renderer* _renderer;

    public:
        void schedule_jobs(
            un::world& world,
            float deltaTime,
            simulation_chain& chain
        ) override {
        }
    };
}
#endif
