#include <cxxopts.hpp>
#include <unnecessary/application.h>
#include <unnecessary/systems/world.h>
#include <unnecessary/systems/transform.h>
#include <unnecessary/graphics/systems/projection.h>
#include <unnecessary/graphics/pipeline/graphics_pipeline.h>
#include <unnecessary/systems/cameras.h>
#include <unnecessary/jobs/loading_jobs.h>
#include <unnecessary/jobs/load_file_job.h>
#include <unnecessary/logging.h>


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
    un::PhongRenderingPipeline* pPipeline = renderer.createPipeline<un::PhongRenderingPipeline>();
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
        un::Buffer phongFragment, phongVertex;
        u32 loadFrag, loadVert;
        std::vector<un::Mesh*> meshes;
        {
            un::JobChain chain(&app.getJobSystem());
            chain.immediately<un::LoadFileJob>(
                &loadFrag,
                std::filesystem::absolute("resources/shaders/phong.frag.spv"),
                &phongFragment
            );
            chain.immediately<un::LoadFileJob>(
                &loadVert,
                std::filesystem::absolute("resources/shaders/phong.vert.spv"),
                &phongVertex
            );
            chain.after<un::LambdaJob>(
                {loadFrag, loadVert},
                [&]() {
                    const vk::Device& device = renderer.getVirtualDevice();
                    auto frag = new un::ShaderStage(
                        "phong-frag",
                        vk::ShaderStageFlagBits::eFragment,
                        device,
                        phongFragment
                    );
                    auto vert = new un::ShaderStage(
                        "phong-vert",
                        vk::ShaderStageFlagBits::eVertex,
                        device,
                        phongVertex,
                        un::PushConstants(0, sizeof(un::PerObjectData))
                    );
                    un::GraphicsPipelineBuilder builder(
                        un::BoundVertexLayout(vertexLayout),
                        {vert, frag}
                    );
                    builder.withStandardRasterization();
                    const un::Pipeline& pipeline = builder.build(
                        renderer,
                        pPipeline->unsafeGetFrameGraph().getVulkanPass()
                    );
                }
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
        //un::systems::add_rendering_systems(app, world);
        LOG(INFO) << "Starting app";
        app.execute();
    }

}