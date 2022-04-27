#include <unnecessary/simulation/world.h>
#include <unnecessary/rendering/renderer.h>
#include <unnecessary/rendering/render_group.h>
#include <unnecessary/simulation/jobs/simulation_worker.h>
#include <unnecessary/simulation/transforms/components.h>
#include <unnecessary/rendering/jobs/graphics_worker.h>
#include <unnecessary/rendering/phong/phong_rendering_pipeline.h>
#include <unnecessary/jobs/job_system.h>
#include <unnecessary/rendering/jobs/render_graph_system.h>
#include <unnecessary/rendering/jobs/render_thread.h>

namespace un {
    typedef un::JobSystem<un::JobWorker, un::SimulationWorker, un::GraphicsWorker> UnnecessaryJobSystem;
}

int main(int argc, char** argv) {
    un::World world;
    un::Application app;

    {
        un::Window window = un::Window::withSize("Orbital", app, un::Size2D(1080, 900));
        un::Renderer renderer = un::Renderer(
            &window,
            "Orbital",
            un::Version(0, 1, 0)
        );
        un::UnnecessaryJobSystem jobSystem(
            std::make_tuple(
                un::WorkerPoolConfiguration<un::JobWorker>::forwarding(4),
                un::WorkerPoolConfiguration<un::SimulationWorker>::forwarding(4),
                un::WorkerPoolConfiguration<un::GraphicsWorker>(
                    1,
                    [&renderer](
                        std::size_t index
                    ) {
                        return new un::GraphicsWorker(
                            &renderer,
                            index,
                            true
                        );
                    }
                )
            )
        );
        un::PhongRenderingPipeline pipeline;
        world.addSystem(new un::RenderGraphSystem<un::UnnecessaryJobSystem>());
        un::RenderThread<un::UnnecessaryJobSystem> renderThread(&jobSystem, &renderer);
        renderer.usePipeline(&pipeline);
        un::Simulator<un::UnnecessaryJobSystem> simulator(&jobSystem, &world);
        app.extend(simulator);
        app.extend(renderThread);
        app.extend(window);
        app.start();
        app.waitExit();
        app.stop();
    }
}