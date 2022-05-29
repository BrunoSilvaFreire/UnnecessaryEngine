#include <unnecessary/simulation/world.h>
#include <unnecessary/rendering/renderer.h>
#include <unnecessary/rendering/render_group.h>
#include <unnecessary/simulation/jobs/simulation_worker.h>
#include <unnecessary/simulation/transforms/components.h>
#include <unnecessary/rendering/jobs/graphics_worker.h>
#include <unnecessary/jobs/job_system.h>
#include <unnecessary/rendering/jobs/render_graph_system.h>
#include <unnecessary/rendering/jobs/render_thread.h>
#include <unnecessary/rendering/basic/basic_rendering_pipeline.h>
#include <unnecessary/misc/files.h>

#include "draw_triangle_pass.h"
#include <triangle.gen.h>
#include <pipelinedescription.serializer.generated.h>
namespace un {
    typedef un::JobSystem<JobWorker, un::SimulationWorker, un::GraphicsWorker> UnnecessaryJobSystem;
}

int main(int argc, char** argv) {
    un::World world;
    un::Application app;
    {
        un::Window window = un::Window::withSize("Triangle", app, un::Size2D(1080, 900));
        un::Renderer renderer = un::Renderer(
            &window,
            "Orbital",
            un::Version(0, 1, 0)
        );
        un::UnnecessaryJobSystem jobSystem(
            std::make_tuple(
                un::WorkerPoolConfiguration<JobWorker>::forwarding(4),
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
        world.addSystem(new un::RenderGraphSystem<un::UnnecessaryJobSystem>());
        auto* pipeline = new un::BasicRenderingPipeline();

        const un::Size2D& windowSize = window.getWindowSize();
        pipeline->enqueueExtraPass(
            std::make_unique<un::DrawTrianglePass>(
                vk::Rect2D(vk::Offset2D(), vk::Extent2D(windowSize.x, windowSize.y)),
                std::vector<vk::ClearValue>(
                    {
                        vk::ClearColorValue(std::array<float, 4>({0, 0, 0, 0})),
                        vk::ClearDepthStencilValue(0, 0)
                    }
                ),
                pipeline->getColor(),
                1
            )
        );
        renderer.usePipeline(pipeline);


        un::RenderThread<un::UnnecessaryJobSystem> renderThread(&jobSystem, &renderer);
        un::Simulator<un::UnnecessaryJobSystem> simulator(&jobSystem, &world);
        app.extend(simulator);
        app.extend(renderThread);
        app.extend(window);
        app.start();
        app.waitExit();
        app.stop();
    }
}