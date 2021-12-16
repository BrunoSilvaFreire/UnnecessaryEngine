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
#include "unnecessary/graphics/rendering/frame_graph.h"
#include <unnecessary/components/dummy.h>
#include <unnecessary/graphics/systems/drawing.h>
#include "gameplay.h"
#include "operations.h"

float randomFloat() {
    return static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
}
/*

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
        explicit IncrementPairJob(
            std::vector<IncrementPair>&& pairs
        ) : pairs(std::move(pairs)) {}

        void operator()(size_t index, un::JobWorker* worker) override {
            IncrementPair& pair = pairs[index];
            pair.position += pair.velocity;
        }
    };
    for (size_t i = 0; i < numEntries; ++i) {
        auto& vel = incrementPairs[i];
        vel.velocity = glm::vec3(
            randomFloat(),
            randomFloat(),
            randomFloat()
        );
    }

    IncrementPairJob job(std::move(incrementPairs));
    un::ParallelForJob::schedule(job, &jobs, numEntries);
}
*/

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
    un::ObjMeshData data;
    un::MeshInfo info;
    un::VertexLayout vertexLayout;
    //Position
    vertexLayout.push<f32>(
        3,
        vk::Format::eR32G32B32Sfloat,
        un::CommonVertexAttribute::ePosition
    );
    //Normal
/*
    vertexLayout.push<f32>(
        3,
        vk::Format::eR32G32B32Sfloat,
        un::CommonVertexAttribute::eNormal
    );
*/

    /*
    //Color
    vertexLayout.push<f32>(3, vk::Format::eR32G32B32Sfloat);
    */
    auto renderingPipeline = renderer.createPipeline<un::PhongRenderingPipeline>();
    world.addSystem<un::TransformSystem>();
    world.addSystem<un::ProjectionSystem>();
#ifndef HEADLESS
    world.addSystem<un::PrepareFrameGraphSystem>(&renderer);
    world.addSystem<un::CameraSystem>(&renderer);
    auto lightingSystem = world.addSystem<un::LightingSystem>(4, &renderer);
    auto drawingSystem = world.addSystem<un::DrawingSystem>(&renderer);
#endif
    world.addSystem<un::RandomizerSystem>();
    u32 load, upload;
    vk::Device device = renderer.getVirtualDevice();

#ifdef HEADLESS
    const int numSidePots = 1000;
#else
    const int numSidePots = 50;
#endif
    const int distance = 15;
    const float finalPoints = -(distance * numSidePots);
#ifndef HEADLESS
    un::JobChain(&jobs)
        .immediately<un::LoadObjJob>(&load, "shaders/teapot.obj", &data)
        .after<un::UploadMeshJob>(load, &upload, vertexLayout, 0, &data, &info, &renderer)
        .after(
            upload,
            [&](un::JobWorker* worker) {
                un::BoundVertexLayout boundLayout(vertexLayout);
                auto* vertex = new un::ShaderStage(
                    "phong.vert",
                    vk::ShaderStageFlagBits::eVertex, vk::Device(),
                    device,
                    un::PushConstants(0, sizeof(un::PerObjectData))
                );
                auto* geometry = new un::ShaderStage(
                    "phong.geom",
                    vk::ShaderStageFlagBits::eGeometry,
                    device
                );
                auto* fragment = new un::ShaderStage(
                    "phong.frag",
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

                pipeline.addDescriptorSet(
                    lightingSystem->getLightsSetLayout()->getVulkanLayout()
                );
                pipeline.addDescriptorSet(
                    drawingSystem->getCameraDescriptorSetAllocator()->getVulkanLayout()
                );
                pipeline.addDescriptorSet(
                    drawingSystem->getObjectDescriptorAllocator()->getVulkanLayout()
                );
                auto* shader = new un::Pipeline(
                    pipeline.build(
                        renderer,
                        renderingPipeline->unsafeGetFrameGraph().getVulkanPass())
                );
                const auto& db = app.getDatabase();
                for (int x = -numSidePots; x <= numSidePots; ++x) {
                    for (int y = -numSidePots; y <= numSidePots; ++y) {
                        entt::entity entity = world.createEntity<
                            un::LocalToWorld,
                            un::Translation,
                            un::Rotation,
                            un::Scale,
                            un::Randomizer,
                            un::RenderMesh,
                            un::ObjectLights
                        >();
                        std::stringstream str;
                        str << "TeapotLights-";
                        //str << x << "-" << y << "-";
                        str << static_cast<u32>(entity);
                        std::string name = str.str();
                        auto[pos, mesh, objectLights, randomizer] = world.get<un::Translation, un::RenderMesh, un::ObjectLights, un::Randomizer>(
                            entity
                        );
                        randomizer.time = 0;
                        randomizer.speed = randomFloat() * .5;
                        randomizer.rotationAxis = glm::vec3(
                            randomFloat(),
                            randomFloat(),
                            randomFloat()
                        );
                        pos.value = glm::vec3(
                            x * distance,
                            0,
                            y * distance
                        );
                        objectLights.descriptorSet = drawingSystem
                            ->getObjectDescriptorAllocator()
                            ->allocate();
                        un::tag(
                            objectLights.descriptorSet,
                            device,
                            db,
                            name + "-DescriptorSet"
                        );
                        auto olb = objectLights.buffer = un::ResizableBuffer(
                            renderer,
                            vk::BufferUsageFlagBits::eUniformBuffer,
                            sizeof(un::ObjectLightingData),
                            true,
                            vk::MemoryPropertyFlagBits::eHostCoherent |
                            vk::MemoryPropertyFlagBits::eHostVisible
                        );

                        olb.tag(device, db, name);
                        un::DescriptorWriter(&renderer)
                            .updateUniformDescriptor(
                                objectLights.descriptorSet,
                                0,
                                olb,
                                vk::DescriptorType::eUniformBuffer
                            );
                        mesh.material = new un::Material(shader);
                        mesh.meshInfo = &info;
                    }
                }
            }
        );
#else
    un::JobChain(&jobs)
        .immediately<un::LoadObjJob>(&load, "resources/teapot.obj", &data)
        .after<un::UploadMeshJob>(load, &upload, vertexLayout, 0, &data, &info, &renderer)
        .after(
            upload,
            [&](un::JobWorker* worker) {
                const auto& db = app.getDatabase();
                for (int x = -numSidePots; x <= numSidePots; ++x) {
                    for (int y = -numSidePots; y <= numSidePots; ++y) {
                        entt::entity entity = world.createEntity<
                            un::LocalToWorld,
                            un::Translation,
                            un::Rotation,
                            un::Scale,
                            un::Randomizer
                        >();
                        std::stringstream str;
                        str << "TeapotLights-";
                        //str << x << "-" << y << "-";
                        str << static_cast<u32>(entity);
                        std::string name = str.str();
                        auto[pos, randomizer] = world.get<un::Translation, un::Randomizer>(
                            entity
                        );
                        randomizer.time = 0;
                        randomizer.speed = randomFloat() * 10;
                        randomizer.rotationAxis = glm::vec3(
                            randomFloat(),
                            randomFloat(),
                            randomFloat()
                        );
                        pos.value = glm::vec3(
                            x * distance,
                            0,
                            y * distance
                        );
                    }
                }
            }
        );
#endif

    for (int j = 0; j < 7; ++j) {
        auto light = world.createEntity<un::LocalToWorld, un::PointLight, un::Translation>();
        auto& pointLight = world.get<un::PointLight>(light);
        pointLight.lighting = un::Lighting(
            1, 0, 0, 1
        );
    }
    auto cameraEntity = world.createEntity<un::LocalToWorld,
        un::Camera,
        un::Projection,
        un::Perspective,
        un::Translation,
        un::Rotation,
        un::Path>();
    un::Path& path = registry.get<un::Path>(cameraEntity);
    path.speed = .5;
    path.positions = {
        un::Location(glm::vec3(0, 0, -150), glm::vec3(0, 0, 0)),
        un::Location(glm::vec3(10, 0, -100), glm::vec3(0, 0, 0)),
        un::Location(glm::vec3(-10, 0, -100), glm::vec3(0, 0, 0)),
        un::Location(glm::vec3(10, -20, -50), glm::vec3(-30, 0, 0)),
        un::Location(glm::vec3(-10, -20, -50), glm::vec3(-30, 0, 0)),
        un::Location(glm::vec3(10, -20, -150), glm::vec3(-30, 0, 0)),
        un::Location(glm::vec3(-10, -20, -150), glm::vec3(-30, 0, 0))
    };
    un::Camera& camera = registry.get<un::Camera>(cameraEntity);
    un::Perspective& perspective = registry.get<un::Perspective>(cameraEntity);
    un::Translation& translation = registry.get<un::Translation>(cameraEntity);
    translation.value.z = -15;
    translation.value.x = 0;
    translation.value.y = 50;
    perspective.aspect = 16.0F / 9.0F;
    perspective.fieldOfView = 100.0F;
    perspective.zNear = 0.1F;
    perspective.zFar = 1000.0F;
#ifndef HEADLESS
    auto cDescriptorSet = camera.cameraDescriptorSet = drawingSystem->getCameraDescriptorSetAllocator()
                                                                    ->allocate();
    auto cDescriptorBuffer = camera.cameraDescriptorBuffer = un::GPUBuffer(
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
#endif
    //world.addSystem<un::FreeFlightSystem>(app.getWindow());
#ifndef HEADLESS
    world.addSystem<un::DispatchFrameGraphSystem>(&renderer);
#endif
    world.addSystem<un::PathRunningSystem>();
    world.getSimulation().getSimulationGraph().saveToDot("simulation.dot");
    app.execute();
}

un::Location::Location(const glm::vec3& position, const glm::quat& rotation) : position(
    position
), rotation(rotation) {}
