#include <unnecessary/simulation/systems/transform_system.h>

namespace un {
    void transform_system::schedule_jobs(
        un::world& world,
        float deltaTime,
        simulation_chain& chain
    ) {
        std::vector<entt::entity> all;
        world.view<local_to_world>().each(
            [&](entt::entity e, local_to_world&) {
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
