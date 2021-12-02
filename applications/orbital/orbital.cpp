#include <cxxopts.hpp>
#include <unnecessary/application.h>
#include "unnecessary/systems/world.h"
#include "unnecessary/systems/transform.h"
#include "unnecessary/graphics/systems/projection.h"
#include "unnecessary/systems/cameras.h"
#include "unnecessary/jobs/loading_jobs.h"
#include "unnecessary/jobs/load_file_job.h"
#include "unnecessary/logging.h"

namespace un {
    namespace systems {
        void add_rendering_systems(un::Application& app, un::World& world) {
            un::Renderer& renderer = app.getRenderer();
            world.addSystem<un::TransformSystem>();
            world.addSystem<un::ProjectionSystem>();
            world.addSystem<un::PrepareFrameGraphSystem>(&renderer);
            world.addSystem<un::CameraSystem>(&renderer);
            world.addSystem<un::DispatchFrameGraphSystem>(&renderer);
        }
    }
}

int main(int argc, char** argv) {
    cxxopts::Options options("UnnecessaryEngine", "Fast Data Oriented Rendering Engine");
    const char* nThreadsName = "nThreads";
    options.add_options()
               (
                   nThreadsName,
                   "Number of threads used by the job systems, defaults to the number of cores.",
                   cxxopts::value<int>()
               );
    auto result = options.parse(argc, argv);
    int nThreads = -1;
    if (result.count(nThreadsName) > 0) {
        nThreads = result[nThreadsName].as<int>();
    }
    un::Application app("Orbital", un::Version(0, 1, 0), nThreads);
    un::Renderer& renderer = app.getRenderer();
    renderer.createPipeline<un::PhongRenderingPipeline>();
    un::World world(app);
    un::VertexLayout vertexLayout;
    vertexLayout.push<float>(
        3,
        vk::Format::eR32G32B32A32Sfloat,
        un::CommonVertexAttribute::ePosition
    );
    vertexLayout.push<float>(
        3,
        vk::Format::eR32G32B32A32Sfloat,
        un::CommonVertexAttribute::eNormal
    );
    {
        u32 load, upload;
        un::Mesh data;
        un::MeshInfo info;
        std::vector<un::Mesh*> meshes;
        un::Buffer phongFragment, phongVertex;
        {
            un::JobChain chain(&app.getJobSystem());
            chain.immediately<un::LoadFileJob>(
                "resources/shaders/phong.frag.spv",
                &phongFragment
            );
            chain.immediately<un::LoadFileJob>(
                "resources/shaders/phong.vert.spv",
                &phongFragment
            );
            // Load model
            chain.immediately<un::LoadModelJob>(
                     &load,
                     "resources/teapot.obj",
                     &meshes,
                     vertexLayout
                 )
                 .after<un::UploadMeshJob>(
                     load,
                     &upload,
                     vertexLayout,
                     0,
                     &data,
                     &info,
                     &renderer
                 );
            chain.onFinished(
                [&](un::JobWorker*) {
                    LOG(INFO) << "Finished loading execution";
                }
            );
        }
    }
    //un::systems::add_rendering_systems(app, world);
    LOG(INFO) << "Starting app";
    app.execute();
}