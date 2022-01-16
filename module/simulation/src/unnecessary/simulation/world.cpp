#include <unnecessary/simulation/world.h>
#include <unnecessary/simulation/systems/system.h>
#include <unnecessary/jobs/worker_chain.h>
#include <unnecessary/simulation/jobs/simulation_worker.h>

namespace un {
    void World::step(f32 deltaTime, un::SimulationChain& chain) {
        un::SimulationChain masterChain;
        _simulationGraph.each(
            [&](u32 index, const un::SimulationNode& node) {
                switch (node.type) {
                    case SimulationNode::eSystem:
                        break;
                    case SimulationNode::eStage:
                        return;
                }
                un::System* pSystem = node.asSystem;
                un::SimulationChain chain;
                pSystem->scheduleJobs(
                    *this,
                    deltaTime,
                    chain
                );
                masterChain.include(chain);
            },
            [](u32 from, u32 to, const un::DependencyType& e) {

            }
        );
    }
}