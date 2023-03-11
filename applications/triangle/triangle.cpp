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

namespace un
{
    using unnecessary_job_system = job_system<job_worker, simulation_worker, graphics_worker>;
}

int main(int argc, char** argv)
{
    un::world world;
    un::application app;
    {
        un::window window = un::window::with_size("Triangle", app, un::size2d(1080, 900));
        auto renderer = un::renderer(
            &window,
            "Orbital",
            un::version(0, 1, 0)
        );
        un::unnecessary_job_system jobSystem(
            std::make_tuple(
                un::worker_pool_configuration<un::job_worker>::forwarding(4),
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
        world.add_system(new un::render_graph_system<un::unnecessary_job_system>());
        auto* pipeline = new un::basic_rendering_pipeline();

        const un::size2d& windowSize = window.get_window_size();
        pipeline->enqueue_extra_pass(
            std::make_unique<un::draw_triangle_pass>(
                vk::Rect2D(vk::Offset2D(), vk::Extent2D(windowSize.x, windowSize.y)),
                std::vector<vk::ClearValue>(
                    {
                        vk::ClearColorValue(std::array<float, 4>({0, 0, 0, 0})),
                        vk::ClearDepthStencilValue(0, 0)
                    }
                ),
                pipeline->get_color(),
                1
            )
        );
        renderer.use_pipeline(pipeline);


        un::render_thread<un::unnecessary_job_system> renderThread(&jobSystem, &renderer);
        un::simulator<un::unnecessary_job_system> simulator(&jobSystem, &world);
        app.extend(simulator);
        app.extend(renderThread);
        app.extend(window);
        app.start();
        app.wait_exit();
        app.stop();
    }
}
