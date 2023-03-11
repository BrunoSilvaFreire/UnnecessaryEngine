//
// Created by brunorbsf on 31/05/22.
//

#ifndef UNNECESSARYENGINE_JOB_VISUALIZATION_H
#define UNNECESSARYENGINE_JOB_VISUALIZATION_H

#include <filesystem>
#include <unnecessary/jobs/job_system.h>
#include <grapphs/graph_writer.h>

namespace un {

    template<typename t_job_system>
    gpp::graph_writer<un::job_graph::inner_graph>
    create_job_system_graph_writer(const t_job_system& jobSystem) {
        auto graph = jobSystem.get_job_graph().get_inner_graph();
        gpp::graph_writer<un::job_graph::inner_graph> writer;
        writer.set_vertex_writer(
            [&jobSystem](std::stringstream& ss, u32 index, const un::job_node& jobNode) {
                t_job_system::for_each_archetype(
                    [&]<typename worker_type, std::size_t WorkerIndex>() {
                        if (WorkerIndex == jobNode.archetypeIndex) {
                            auto& pool = jobSystem.template get_worker_pool<worker_type>();
                            auto job = pool.get_job(jobNode.poolLocalIndex);
                            if (job == nullptr) {
                                ss << " [shape=box label=\"#" << index << ": Job Not Found\"];";
                            }
                            else {
                                ss << " [shape=box label=\"#" << index << ": " << job->get_name()
                                   << '"' << "];";
                            }
                        }
                    }
                );

            }
        );
        writer.set_edge_writer(
            [](std::stringstream& ss, u32 from, u32 to, const un::dependency_type& dependencyType) {
                ss << R"([style="bold"];)";
            }
        );
        writer.set_edge_predicate(
            [](u32 from, u32 to, const un::dependency_type& dependencyType) {
                return dependencyType == un::dependency_type::uses;
            }
        );
        writer.add_note(R"(A -> B [label="A depends on B", style="bold"])");
        return writer;
    }
}
#endif
