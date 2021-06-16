#include <unnecessary/application.h>
#include <unnecessary/systems/world.h>
#include <unnecessary/systems/parallel_system_data.h>
#include <unnecessary/systems/transform.h>
#include <unnecessary/graphics/systems/rendering.h>
#include <unnecessary/graphics/graphics_pipeline.h>
#include <unnecessary/jobs/loading_jobs.h>
#include <unnecessary/jobs/job_chain.h>
#include <unnecessary/jobs/parallel_for_job.h>

float randomFloat() {
    return static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
}

int main() {
    un::Application app("Demo", un::Version(0, 1, 0));
    un::World world(app);

    entt::registry &registry = world.getRegistry();
    un::JobSystem &jobs = app.getJobSystem();
    un::MeshData data;
    un::MeshInfo info;
    un::VertexLayout vertexLayout;
    //Position
    vertexLayout.push<f32>(3, vk::Format::eR32G32B32Sfloat);

    /*
    //Color
    vertexLayout.push<f32>(3, vk::Format::eR32G32B32Sfloat);
    */
    auto transformSystem = world.addSystem<un::TransformSystem>();
    auto projectionSystem = world.addSystem<un::ProjectionSystem>();
    auto drawingSystem = new un::DrawingSystem(app.getRenderer());
    auto drawingSystemId = world.addSystem(drawingSystem);
    u32 load, upload;
    un::JobChain(&jobs)
            .immediately<un::LoadObjJob>(&load, "resources/teapot.obj", &data)
            .after<un::UploadMeshJob>(load, &upload, vertexLayout, 0, &data, &info, &app.getRenderer())
            .after(upload, [&](un::JobWorker *worker) {
                vk::Device device = app.getRenderer().getVirtualDevice();

                un::DescriptorLayout descriptorLayout;
                descriptorLayout.withStandardCameraMatrices();
                un::BoundVertexLayout boundLayout(vertexLayout);
                un::PushConstants objDataConstants(0, sizeof(un::PerObjectData));
                un::ShaderStage vertex(
                        "standart.vert",
                        vk::ShaderStageFlagBits::eVertex,
                        descriptorLayout,
                        device,
                        objDataConstants
                );

                un::ShaderStage fragment("standart.frag", vk::ShaderStageFlagBits::eFragment, descriptorLayout, device);
                un::GraphicsPipelineBuilder pipeline(boundLayout, {vertex, fragment});
                pipeline.withStandardRasterization();
                un::Pipeline shader = pipeline.build(app.getRenderer(), drawingSystem->getRenderPass());

                entt::entity entity = world.createEntity<un::LocalToWorld, un::Translation, un::RenderMesh>();
                un::RenderMesh &mesh = registry.get<un::RenderMesh>(entity);
                mesh.material = new un::Material(&shader);
                mesh.mesh = &info;
            });



//    auto renderMeshSystem = world.addSystem<un::RenderMeshSystem>(app.getRenderer(), drawingSystem);
    world.systemMustRunAfter(transformSystem, projectionSystem);
//    world.systemMustRunAfter(renderMeshSystem, transformSystem);
//    world.systemMustRunAfter(drawingSystemId, renderMeshSystem);
//    world.createEntity<un::Translation, un::Rotation, un::Camera, un::Projection, un::Perspective>();
    app.execute();
}