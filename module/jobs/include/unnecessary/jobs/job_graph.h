#include <unnecessary/graphs/dependency_graph.h>
#include <mutex>
#include <ostream>

#ifndef UNNECESSARYENGINE_JOB_GRAPH_H
#define UNNECESSARYENGINE_JOB_GRAPH_H
namespace un {
    struct job_node {
        //The archetype of this job.
        u32 archetypeIndex;
        // This index points into the un::WorkerPool that this job resides.
        u32 poolLocalIndex;

        friend std::ostream& operator<<(std::ostream& os, const job_node& node) {
            os << "archetypeIndex: " << node.archetypeIndex << ", poolLocalIndex: "
               << node.poolLocalIndex;
            return os;
        }
    };

    typedef un::dependency_graph<un::job_node, un::job_handle> job_graph;
}
#endif
