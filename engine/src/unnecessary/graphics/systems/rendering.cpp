
#include <utility>
#include <unnecessary/graphics/systems/rendering.h>
#include <unnecessary/systems/world.h>

namespace un {

    void RenderMeshSystem::step(World &world, f32 delta, un::JobWorker *worker) {
        entt::registry &registry = world.getRegistry();
        auto view = registry.view<un::RenderMesh>();
        for (entt::entity cameraEntity : registry.view<un::Camera>()) {
            un::Camera &camera = registry.get<un::Camera>(cameraEntity);
            u32 count = 0;
            vk::DeviceSize zeroOffset = 0;
            vk::CommandBufferInheritanceInfo inheritanceInfo(
                    camera.renderPass,
                    0,
                    nullptr,
                    false,
                    (vk::QueryControlFlags) 0,
                    (vk::QueryPipelineStatisticFlags) 0
            );

            std::unordered_map<un::Pipeline *, std::vector<entt::entity>> vectors;
            for (entt::entity entity : view) {
                un::RenderMesh &renderMesh = registry.get<un::RenderMesh>(entity);
                un::Pipeline *pipeline = renderMesh.material->getShader();
                vectors[pipeline].push_back(entity);
            }

            std::priority_queue<un::PipelineBatch> batches;
            typedef typename std::unordered_map<un::Pipeline *, std::vector<entt::entity>>::value_type pair_type;
            for (pair_type &pair : vectors) {
                batches.emplace(pair.first, pair.second);
            }
            vk::CommandBuffer commandBuffer = buffer.getVulkanBuffer();
            commandBuffer.begin(vk::CommandBufferBeginInfo(
                    (vk::CommandBufferUsageFlags) vk::CommandBufferUsageFlagBits::eOneTimeSubmit |
                    vk::CommandBufferUsageFlagBits::eRenderPassContinue,
                    &inheritanceInfo
            ));
            while (!batches.empty()) {
                const un::PipelineBatch &batch = batches.top();
                un::Pipeline *pipeline = batch.getPipeline();
                commandBuffer.bindPipeline(
                        vk::PipelineBindPoint::eGraphics,
                        pipeline->getPipeline()
                );
                for (entt::entity entity : batch.getBatched()) {
                    un::RenderMesh &renderMesh = registry.get<un::RenderMesh>(entity);
                    //TODO: Batch meshes
                    un::MeshInfo *mesh = renderMesh.meshInfo;
                    commandBuffer.bindVertexBuffers(
                            0,
                            1,
                            &mesh->getVertex().getVulkanBuffer(),
                            &zeroOffset
                    );
                    commandBuffer.bindIndexBuffer(
                            mesh->getIndex(),
                            (vk::DeviceSize) 0,
                            vk::IndexType::eUint16
                    );
                    un::PerObjectData data{};
                    glm::mat4 modelMatrix;
                    auto localToWorld = registry.try_get<un::LocalToWorld>(entity);
                    if (localToWorld != nullptr) {
                        data.modelMatrix = localToWorld->value;
                    } else {
                        data.modelMatrix = glm::identity<glm::mat4>();
                    }
                    commandBuffer.pushConstants(
                            pipeline->getPipelineLayout(),
                            vk::ShaderStageFlagBits::eAllGraphics,
                            0,
                            sizeof(un::PerObjectData),
                            &data
                    );
                    commandBuffer.drawIndexed(
                            mesh->getIndexCount(),
                            1,
                            0,
                            0,
                            0
                    );
                }
                batches.pop();
            }
            commandBuffer.end();
        }
    }

    RenderMeshSystem::RenderMeshSystem(
            Renderer &renderer,
            un::DrawingSystem *system
    ) : drawingSystem(system), buffer(renderer) {
    }

    void ProjectionSystem::step(World &world, f32 delta, un::JobWorker *worker) {
        entt::registry &registry = world.getRegistry();
        auto view = registry.view<un::Camera, un::Projection, un::Perspective>();
        for (entt::entity entity : view) {
            un::Camera &camera = registry.get<un::Camera>(entity);
            un::Projection &projectile = registry.get<un::Projection>(entity);
            un::Perspective &perspective = registry.get<un::Perspective>(entity);
            projectile.value = glm::perspective(
                    perspective.fieldOfView,
                    perspective.aspect,
                    perspective.zNear,
                    perspective.zFar
            );
        }
    }

    const std::vector<entt::entity> &PipelineBatch::getBatched() const {
        return batched;
    }

    Pipeline *PipelineBatch::getPipeline() const {
        return pipeline;
    }

    PipelineBatch::PipelineBatch(un::Pipeline *pipeline, std::vector<entt::entity> batched) : pipeline(pipeline),
                                                                                              batched(std::move(
                                                                                                      batched)) {}

    bool PipelineBatch::operator<(const PipelineBatch &rhs) const {
        return pipeline->getOrder() < rhs.pipeline->getOrder();
    }

    bool PipelineBatch::operator>(const PipelineBatch &rhs) const {
        return rhs < *this;
    }

    bool PipelineBatch::operator<=(const PipelineBatch &rhs) const {
        return !(rhs < *this);
    }

    bool PipelineBatch::operator>=(const PipelineBatch &rhs) const {
        return !(*this < rhs);
    }
}