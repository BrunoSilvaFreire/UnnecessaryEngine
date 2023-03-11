#include <unnecessary/simulation/world.h>
#include <unnecessary/simulation/systems/system.h>
#include <unnecessary/jobs/worker_chain.h>
#include <unnecessary/simulation/jobs/simulation_worker.h>

namespace un {
    void world::step(f32 deltaTime, simulation_chain& chain) {
        simulation_chain masterChain;
        _simulationGraph.each(
            [&](u32 index, const simulation_node& node) {
                switch (node.type) {
                    case simulation_node::type::system:
                        break;
                    case simulation_node::type::stage:
                        return;
                }
                system* pSystem = node.asSystem;
                simulation_chain chain;
                pSystem->schedule_jobs(
                    *this,
                    deltaTime,
                    chain
                );
                masterChain.include(chain);
            },
            [](u32 from, u32 to, const dependency_type& e) {
            }
        );
    }
}
