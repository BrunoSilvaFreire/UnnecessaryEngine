#ifndef UNNECESSARYENGINE_SIMULATION_GRAPH_H
#define UNNECESSARYENGINE_SIMULATION_GRAPH_H

#include <string>
#include <unnecessary/graphs/dependency_graph.h>
#include <unnecessary/simulation/jobs/simulation_worker.h>

namespace un {
    class System;

    typedef un::WorkerChain<un::SimulationWorker> SimulationChain;

    struct SimulationNode {
        enum Type {
            eSystem,
            eStage
        };
        std::string name;
        Type type;
        union {
            un::System* asSystem;
            struct {
                // Reserved for future use
            } asStage;
        };
    };

    class SimulationGraph : public un::DependencyGraph<un::SimulationNode> {

    };
}
#endif
