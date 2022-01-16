//
// Created by bruno on 21/12/2021.
//
#include <unnecessary/simulation/transforms/compute_matrices.h>

namespace un {

    void ComputeLocalToWorldJob::operator()(size_t index, un::SimulationWorker* worker) {
        entt::entity entity = entities[index];
        un::LocalToWorld& ltw = world->get<un::LocalToWorld>(entity);
        glm::mat4 matrix = glm::identity<glm::mat4>();
        if (auto translation = world->try_get<un::Translation>(entity); translation) {
            matrix = glm::translate(matrix, translation->value);
        }
        if (auto rotation = world->try_get<un::Rotation>(entity); rotation) {
            matrix *= glm::toMat4(rotation->value);
        }
        if (auto scale = world->try_get<un::Scale>(entity); scale) {
            matrix = glm::scale(matrix, scale->value);
        }
        ltw.value = matrix;
    }

    ComputeLocalToWorldJob::ComputeLocalToWorldJob(
        World* world,
        const std::vector<entt::entity>& entities
    ) : world(world), entities(entities) {}

}