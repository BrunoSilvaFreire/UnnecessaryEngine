//
// Created by brunorbsf on 31/05/22.
//

#ifndef UNNECESSARYENGINE_JOB_VISUALIZATION_H
#define UNNECESSARYENGINE_JOB_VISUALIZATION_H

#include <filesystem>
#include <unnecessary/jobs/job_system.h>
#include <grapphs/graph_writer.h>

namespace un {

    template<typename TJobSystem>
    gpp::graph_writer<un::JobGraph::InnerGraph>
    create_job_system_graph_writer(const TJobSystem& jobSystem) {
        auto graph = jobSystem.getJobGraph().getInnerGraph();
        gpp::graph_writer<un::JobGraph::InnerGraph> writer;
        writer.set_vertex_writer(
            [&jobSystem](std::stringstream& ss, u32 index, const un::JobNode& jobNode) {
                TJobSystem::for_each_archetype(
                    [&]<typename WorkerType, std::size_t WorkerIndex>() {
                        if (WorkerIndex == jobNode.archetypeIndex) {
                            auto& pool = jobSystem.template getWorkerPool<WorkerType>();
                            auto job = pool.getJob(jobNode.poolLocalIndex);
                            if (job == nullptr) {
                                ss << " [shape=box label=\"#" << index << ": Job Not Found\"];";
                            } else {
                                ss << " [shape=box label=\"#" << index << ": " << job->getName() << '"' << "];";
                            }
                        }
                    }
                );

            }
        );
        writer.set_edge_writer(
            [](std::stringstream& ss, u32 from, u32 to, const un::DependencyType& dependencyType) {
                ss << R"([style="bold"];)";
            }
        );
        writer.set_edge_predicate(
            [](u32 from, u32 to, const un::DependencyType& dependencyType) {
                return dependencyType == un::DependencyType::eUses;
            }
        );
        writer.add_note(R"(A -> B [label="A depends on B", style="bold"])");
        return writer;
    }
}
#endif
