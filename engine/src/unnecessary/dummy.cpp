#include <unnecessary/application.h>
#include <unnecessary/systems/world.h>
#include <unnecessary/systems/transform.h>
#include <unnecessary/systems/cameras.h>
#include <unnecessary/systems/lighting.h>
#include <unnecessary/graphics/systems/rendering.h>
#include <unnecessary/graphics/pipeline/graphics_pipeline.h>
#include <unnecessary/graphics/descriptors/descriptor_allocator.h>
#include <unnecessary/jobs/loading_jobs.h>
#include <unnecessary/jobs/job_chain.h>
#include <unnecessary/jobs/parallel_for_job.h>
#include <cxxopts.hpp>
#include <unnecessary/graphics/lighting.h>

float randomFloat() {
    return static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
}

void testParallelism(un::JobSystem &jobs) {
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
        IncrementPairJob(std::vector<IncrementPair> &&pairs) : pairs(std::move(this->pairs)) {}

        void operator()(size_t index, un::JobWorker *worker) override {
            IncrementPair &pair = pairs[index];
            pair.position += pair.velocity;
        }
    };
    for (size_t i = 0; i < numEntries; ++i) {
        auto &vel = incrementPairs[i];
        vel.velocity = glm::vec3(
                randomFloat(),
                randomFloat(),
                randomFloat()
        );
    }

    IncrementPairJob job(std::move(incrementPairs));
    job.schedule(&jobs, numEntries);
}

int main(int argc, char **argv) {
    cxxopts::Options options("UnnecessaryEngine", "Fast Data Oriented Rendering Engine");
    const char *nThreadsName = "nThreads";
    options.add_options()
            (nThreadsName, "Number of threads used by the job systems, defaults to the number of cores.",
             cxxopts::value<int>());
    auto result = options.parse(argc, argv);
    int nThreads = -1;
    if (result.count(nThreadsName) > 0) {
        nThreads = result[nThreadsName].as<int>();
    }
    un::Application app("Demo", un::Version(0, 1, 0), nThreads);
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
    un::Renderer &renderer = app.getRenderer();
    auto drawingSystem = new un::DrawingSystem(renderer);
    auto drawingSystemId = world.addSystem(drawingSystem);
    u32 load, upload;
    vk::Device device = renderer.getVirtualDevice();
    un::JobChain(&jobs)
            .immediately<un::LoadObjJob>(&load, "resources/teapot.obj", &data)
            .after<un::UploadMeshJob>(load, &upload, vertexLayout, 0, &data, &info, &renderer)
            .after(upload, [&](un::JobWorker *worker) {
                un::BoundVertexLayout boundLayout(vertexLayout);
                un::ShaderStage *vertex = new un::ShaderStage(
                        "standart.vert",
                        vk::ShaderStageFlagBits::eVertex,
                        device,
                        un::PushConstants(0, sizeof(un::PerObjectData))
                );
                vertex->usesDescriptor(0, 0);


                auto *fragment = new un::ShaderStage(
                        "standart.frag",
                        vk::ShaderStageFlagBits::eFragment,
                        device);
                fragment->usesDescriptor(0, 1);
                fragment->usesDescriptor(1, 0);
                auto *geometry = new un::ShaderStage(
                        "standart.geom",
                        vk::ShaderStageFlagBits::eGeometry,
                        device
                );
                un::GraphicsPipelineBuilder pipeline(boundLayout, {vertex, geometry, fragment});
                pipeline.withStandardRasterization();
                un::DescriptorSetLayout fragmentDescriptorLayout;
                fragmentDescriptorLayout.push<un::ObjectLightingData>(
                        "objectLighting",
                        vk::DescriptorType::eUniformBuffer
                );
                pipeline.addDescriptorSet(std::move(fragmentDescriptorLayout));
                auto *shader = new un::Pipeline(pipeline.build(renderer, drawingSystem->getRenderPass()));
                for (int i = 0; i < 3; ++i) {
                    entt::entity entity = world.createEntity<un::LocalToWorld, un::Translation, un::RenderMesh>();
                    un::Translation &pos = registry.get<un::Translation>(entity);
                    pos.value.x = i * 5;
                    un::RenderMesh &mesh = registry.get<un::RenderMesh>(entity);
                    mesh.material = new un::Material(shader);
                    mesh.meshInfo = &info;
                }
            });


    un::DescriptorSetLayout cameraLayout;
    cameraLayout.withStandardCameraMatrices();
    //cameraLayout.withGlobalSceneLighting();
    un::DescriptorAllocator cameraDescriptorAllocator(
            std::move(cameraLayout),
            device,
            1,
            vk::ShaderStageFlagBits::eVertex
    );

    auto cameraEntity = world.createEntity<un::LocalToWorld, un::Camera, un::Projection, un::Perspective, un::Translation>();
    un::Camera &camera = registry.get<un::Camera>(cameraEntity);
    un::Perspective &perspective = registry.get<un::Perspective>(cameraEntity);
    un::Translation &translation = registry.get<un::Translation>(cameraEntity);
    translation.value.z = -5;
    perspective.aspect = 16.0F / 9.0F;
    perspective.fieldOfView = 100.0F;
    perspective.zNear = 0.1F;
    perspective.zFar = 1000.0F;
    auto cDescriptorSet = camera.cameraDescriptorSet = cameraDescriptorAllocator.allocate();
    auto cDescriptorBuffer = camera.cameraDescriptorBuffer = un::Buffer(
            renderer,
            vk::BufferUsageFlagBits::eUniformBuffer,
            sizeof(un::Matrices),
            true,
            vk::MemoryPropertyFlagBits::eHostCoherent | vk::MemoryPropertyFlagBits::eHostVisible
    );
    vk::DescriptorBufferInfo bufferInfo(
            cDescriptorBuffer,
            0,
            cDescriptorBuffer.getSize()
    );
    device.updateDescriptorSets(
            {
                    vk::WriteDescriptorSet(
                            cDescriptorSet,
                            0,
                            0,
                            1,
                            vk::DescriptorType::eUniformBuffer,
                            nullptr,
                            &bufferInfo
                    )
            },
            {}
    );
    world.addSystem<un::LightingSystem>(4, &renderer);
    world.addSystem<un::CameraSystem>(&renderer);
    //auto renderMeshSystem = world.addSystem<un::RenderMeshSystem>(app.getRenderer(), drawingSystem);
    world.systemMustRunAfter(transformSystem, projectionSystem);
//    world.systemMustRunAfter(renderMeshSystem, transformSystem);
//    world.systemMustRunAfter(drawingSystemId, renderMeshSystem);
    app.execute();
}