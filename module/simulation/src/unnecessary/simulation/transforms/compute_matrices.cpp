#include <unnecessary/simulation/transforms/compute_matrices.h>

namespace un {
    void compute_local_to_world_job::operator()(size_t index, simulation_worker* worker) {
        entt::entity entity = _entities[index];
        local_to_world& ltw = _world->get<local_to_world>(entity);
        auto matrix = glm::identity<glm::mat4>();

        if (auto translation = _world->try_get<un::translation>(entity); translation) {
            matrix = translate(matrix, translation->value);
        }

        if (auto rotation = _world->try_get<un::rotation>(entity); rotation) {
            matrix *= toMat4(rotation->value);
        }

        if (auto scale = _world->try_get<un::scale>(entity); scale) {
            matrix = glm::scale(matrix, scale->value);
        }

        ltw.value = matrix;
    }

    compute_local_to_world_job::compute_local_to_world_job(
        world* world,
        const std::vector<entt::entity>& entities
    ) : _world(world), _entities(entities) {
    }
}
