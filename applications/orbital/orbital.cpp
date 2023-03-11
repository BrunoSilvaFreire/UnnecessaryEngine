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

namespace un
{
    using unnecessary_job_system = job_system<job_worker, simulation_worker, graphics_worker>;
}

int main(int argc, char** argv)
{
    un::world world;
    un::application app;

    {
        un::window window = un::window::with_size("Orbital", app, un::size2d(1080, 900));
        auto renderer = un::renderer(
            &window,
            "Orbital",
            un::version(0, 1, 0)
        );
        un::unnecessary_job_system jobSystem(
            std::make_tuple(
                un::worker_pool_configuration<job_worker>::forwarding(4),
                un::worker_pool_configuration<un::simulation_worker>::forwarding(4),
                un::worker_pool_configuration<un::graphics_worker>(
                    1,
                    [&renderer](
                    std::size_t index
                )
                    {
                        return new un::graphics_worker(
                            &renderer,
                            index,
                            true
                        );
                    }
                )
            )
        );
        un::phong_rendering_pipeline pipeline;
        world.add_system(new un::render_graph_system<un::unnecessary_job_system>());
        un::render_thread<un::unnecessary_job_system> renderThread(&jobSystem, &renderer);
        renderer.use_pipeline(&pipeline);
        un::simulator<un::unnecessary_job_system> simulator(&jobSystem, &world);
        app.extend(simulator);
        app.extend(renderThread);
        app.extend(window);
        app.start();
        app.wait_exit();
        app.stop();
    }
}
