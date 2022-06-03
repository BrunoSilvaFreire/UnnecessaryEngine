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
    gpp::GraphWriter<un::JobGraph::InnerGraph>
    create_job_system_graph_writer(const TJobSystem& jobSystem) {
        auto graph = jobSystem.getJobGraph().getInnerGraph();
        gpp::GraphWriter<un::JobGraph::InnerGraph> writer;
        writer.setVertexWriter(
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
        writer.setEdgeWriter(
            [](std::stringstream& ss, u32 from, u32 to, const un::DependencyType& jobNode) {
                ss << " [label=\"" << un::to_string(jobNode) << "\"];";
            }
        );
//        writer.setEdgePredicate(
//            [](u32 from, u32 to, const un::DependencyType& dependencyType) {
//                return dependencyType == un::DependencyType::eUses;
//            }
//        );
        return writer;
    }
}
#endif
