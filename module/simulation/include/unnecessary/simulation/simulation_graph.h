#ifndef UNNECESSARYENGINE_SIMULATION_GRAPH_H
#define UNNECESSARYENGINE_SIMULATION_GRAPH_H

#include <string>
#include <unnecessary/jobs/worker_chain.h>
#include <unnecessary/graphs/dependency_graph.h>
#include <unnecessary/simulation/jobs/simulation_worker.h>

namespace un {
    class system;

    using simulation_chain = worker_chain<simulation_worker>;

    struct simulation_node {
        enum class type {
            system,
            stage
        };

        std::string name;
        type type;

        union {
            system* asSystem;

            struct {
                // Reserved for future use
            } asStage;
        };
    };

    class simulation_graph : public dependency_graph<simulation_node> {
    };
}
#endif
