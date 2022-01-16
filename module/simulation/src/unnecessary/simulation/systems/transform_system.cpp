#include <unnecessary/simulation/systems/transform_system.h>

namespace un {

    void TransformSystem::scheduleJobs(
        World& world,
        float deltaTime,
        SimulationChain& chain
    ) {
        std::vector<entt::entity> all;
        world.view<un::LocalToWorld>().each(
            [&](entt::entity e, un::LocalToWorld&) {
                all.emplace_back(e);
            }
        );
//        un::ComputeLocalToWorldJob::parallelize(
//            new un::ComputeLocalToWorldJob(&world, all),
//            chain,
//            all.size(),
//            128
//        );
    }
}