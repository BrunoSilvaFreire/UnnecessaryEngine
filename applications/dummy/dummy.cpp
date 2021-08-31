#include <unnecessary/application.h>
#include <unnecessary/systems/world.h>
#include <unnecessary/systems/transform.h>
#include <unnecessary/systems/cameras.h>
#include <unnecessary/systems/lighting.h>
#include <unnecessary/systems/graphics_systems.h>
#include <unnecessary/graphics/pipeline/graphics_pipeline.h>
#include <unnecessary/graphics/descriptors/descriptor_allocator.h>
#include <unnecessary/graphics/descriptors/descriptor_writer.h>
#include <unnecessary/graphics/descriptors/descriptor_set.h>
#include <unnecessary/graphics/systems/projection.h>
#include <unnecessary/jobs/loading_jobs.h>
#include <unnecessary/jobs/job_chain.h>
#include <unnecessary/jobs/parallel_for_job.h>
#include <cxxopts.hpp>
#include <unnecessary/graphics/lighting.h>
#include <random>
#include <unnecessary/graphics/frame_graph.h>
#include <unnecessary/components/dummy.h>
#include <unnecessary/graphics/systems/drawing.h>
#include "gameplay.h"

float randomFloat() {
    return static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
}

void testParallelism(un::JobSystem& jobs) {
    int numEntries = 24000000;
    struct IncrementPair {
        glm::vec3 position, velocity;
    };
    std::vector<IncrementPair> incrementPairs;
    incrementPairs.resize(numEntries);
    class IncrementPairJob : public un::ParallelForJob {
    private:
        std::vector<IncrementPair> pairs;
    public:
        IncrementPairJob(std::vector<IncrementPair>&& pairs) : pairs(std::move(this->pairs)) {}

        void operator()(size_t index, un::JobWorker* worker) override {
            IncrementPair& pair = pairs[index];
            pair.position += pair.velocity;
        }
    };
    for (size_t i = 0 ; i < numEntries ; ++i) {
        auto& vel = incrementPairs[i];
        vel.velocity = glm::vec3(
            randomFloat(),
            randomFloat(),
            randomFloat()
        );
    }

    IncrementPairJob job(std::move(incrementPairs));
    job.schedule(&jobs, numEntries);
}

int main(int argc, char** argv) {
    cxxopts::Options options("UnnecessaryEngine", "Fast Data Oriented Rendering Engine");
    const char* nThreadsName = "nThreads";
    options.add_options()
               (
                   nThreadsName,
                   "Number of threads used by the job systems, defaults to the number of cores.",
                   cxxopts::value<int>());
    auto result = options.parse(argc, argv);
    int nThreads = -1;
    if (result.count(nThreadsName) > 0) {
        nThreads = result[nThreadsName].as<int>();
    }
    un::Application app("Demo", un::Version(0, 1, 0), nThreads);
    un::World world(app);
    un::Renderer& renderer = app.getRenderer();
    entt::registry& registry = world.getRegistry();
    un::JobSystem& jobs = app.getJobSystem();
    un::MeshData data;
    un::MeshInfo info;
    un::VertexLayout vertexLayout;
    //Position
    vertexLayout.push<f32>(3, vk::Format::eR32G32B32Sfloat);

    /*
    //Color
    vertexLayout.push<f32>(3, vk::Format::eR32G32B32Sfloat);
    */
    auto renderingPipeline = renderer.createPipeline<un::DummyRenderingPipeline>();
    auto transformSystem = world.addSystem<un::TransformSystem>();
    auto projectionSystem = world.addSystem<un::ProjectionSystem>();
    world.addSystem<un::PrepareFrameGraphSystem>(&renderer);
    world.addSystem<un::LightingSystem>(4, &renderer);
    auto drawingSystem = new un::DrawingSystem(renderer);
    auto drawingSystemId = world.addSystem(drawingSystem);
    u32 load, upload;
    vk::Device device = renderer.getVirtualDevice();
    un::JobChain(&jobs)
        .immediately<un::LoadObjJob>(&load, "resources/teapot.obj", &data)
        .after<un::UploadMeshJob>(load, &upload, vertexLayout, 0, &data, &info, &renderer)
        .after(
            upload,
            [&](un::JobWorker* worker) {
                un::BoundVertexLayout boundLayout(vertexLayout);
                auto* vertex = new un::ShaderStage(
                    "standart.vert",
                    vk::ShaderStageFlagBits::eVertex,
                    device,
                    un::PushConstants(0, sizeof(un::PerObjectData))
                );
                auto* geometry = new un::ShaderStage(
                    "standart.geom",
                    vk::ShaderStageFlagBits::eGeometry,
                    device
                );
                auto* fragment = new un::ShaderStage(
                    "standart.frag",
                    vk::ShaderStageFlagBits::eFragment,
                    device
                );

                vertex->usesDescriptor(1, 0);
                fragment->usesDescriptor(0, 0);
                fragment->usesDescriptor(2, 0);
                un::GraphicsPipelineBuilder pipeline(
                    boundLayout,
                    {vertex, geometry, fragment}
                );
                pipeline.withStandardRasterization();

                pipeline.addDescriptorResource(
                    drawingSystem->getSceneDescriptorAllocator()->getVulkanLayout()
                );
                pipeline.addDescriptorResource(
                    drawingSystem->getCameraDescriptorSetAllocator()->getVulkanLayout()
                );
                pipeline.addDescriptorResource(
                    drawingSystem->getObjectDescriptorAllocator()->getVulkanLayout()
                );
                auto* shader = new un::Pipeline(
                    pipeline.build(
                        renderer,
                        renderingPipeline->unsafeGetFrameGraph().getVulkanPass()                    )
                );
                for (int i = 0 ; i < 1 ; ++i) {


                    entt::entity entity = world.createEntity<un::LocalToWorld, un::Translation, un::RenderMesh, un::ObjectLights, un::Path>();
                    auto[pos, mesh, objectLights] = world.get<un::Translation, un::RenderMesh, un::ObjectLights>(
                        entity
                    );
                /*    path.speed = 1;
                    path.positions = std::vector<glm::vec3>(
                        {
                            glm::vec3(0, 0, 0),
                            glm::vec3(0, 0, -10),
                            glm::vec3(0, 0, 0)
                        }
                    );*/
                    pos.value.x = i * 5;

                    objectLights.descriptorSet = drawingSystem->getObjectDescriptorAllocator()
                                                              ->allocate();
                    objectLights.buffer = un::ResizableBuffer(
                        renderer,
                        vk::BufferUsageFlagBits::eUniformBuffer,
                        sizeof(un::ObjectLightingData),
                        true,
                        vk::MemoryPropertyFlagBits::eHostCoherent |
                        vk::MemoryPropertyFlagBits::eHostVisible
                    );
                    vk::DescriptorBufferInfo bufferInfo(
                        objectLights.buffer,
                        objectLights.buffer.getOffset(),
                        VK_WHOLE_SIZE
                    );
                    device.updateDescriptorSets(
                        {
                            vk::WriteDescriptorSet(
                                objectLights.descriptorSet,
                                0, 0,
                                1,
                                vk::DescriptorType::eUniformBuffer,
                                nullptr,
                                &bufferInfo
                            )
                        }, {

                        }
                    );
                    mesh.material = new un::Material(shader);
                    mesh.meshInfo = &info;
                }
            }
        );

    auto cameraEntity = world.createEntity<un::LocalToWorld, un::Camera, un::Projection, un::Perspective, un::Translation,un::Rotation, un::FreeFlight>();
    un::FreeFlight& freeFlight = registry.get<un::FreeFlight>(cameraEntity);
    freeFlight.speed = 1;
    freeFlight.angularSpeed = 0.05F;
    un::Camera& camera = registry.get<un::Camera>(cameraEntity);
    un::Perspective& perspective = registry.get<un::Perspective>(cameraEntity);
    un::Translation& translation = registry.get<un::Translation>(cameraEntity);
    translation.value.z = -15;
    perspective.aspect = 16.0F / 9.0F;
    perspective.fieldOfView = 100.0F;
    perspective.zNear = 0.1F;
    perspective.zFar = 1000.0F;
    auto cDescriptorSet = camera.cameraDescriptorSet = drawingSystem->getCameraDescriptorSetAllocator()
                                                                    ->allocate();
    auto cDescriptorBuffer = camera.cameraDescriptorBuffer = un::Buffer(
        renderer,
        vk::BufferUsageFlagBits::eUniformBuffer,
        sizeof(un::Matrices),
        true,
        vk::MemoryPropertyFlagBits::eHostCoherent |
        vk::MemoryPropertyFlagBits::eHostVisible
    );
    {
        un::DescriptorWriter writer(&renderer);
        writer.updateUniformDescriptor(
            cDescriptorSet,
            0,
            cDescriptorBuffer,
            vk::DescriptorType::eUniformBuffer
        );
    }
    world.addSystem<un::CameraSystem>(&renderer);
    world.addSystem<un::FreeFlightSystem>(app.getWindow());

    world.addSystem<un::DispatchFrameGraphSystem>(&renderer);
    //world.addSystem<un::PathRunningSystem>();
    //auto renderMeshSystem = world.addSystem<un::RenderMeshSystem>(app.getRenderer(), drawingSystem);
    world.systemMustRunAfter(transformSystem, projectionSystem);



//    world.systemMustRunAfter(renderMeshSystem, transformSystem);
//    world.systemMustRunAfter(drawingSystemId, renderMeshSystem);
    app.execute();
}