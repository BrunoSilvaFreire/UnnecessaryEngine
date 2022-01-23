#include <unnecessary/graphs/dependency_graph.h>
#include <mutex>

#ifndef UNNECESSARYENGINE_JOB_GRAPH_H
#define UNNECESSARYENGINE_JOB_GRAPH_H
namespace un {
    struct JobNode {
        //The archetype of this job.
        u32 archetypeIndex;
        // This index points into the un::WorkerPool that this job resides.
        u32 poolLocalIndex;
    };

    typedef un::DependencyGraph<un::JobNode, un::JobHandle> JobGraph;
}
#endif
