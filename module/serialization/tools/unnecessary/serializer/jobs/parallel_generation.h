//
// Created by bruno on 26/05/2022.
//

#ifndef UNNECESSARYENGINE_PARALLEL_GENERATION_H
#define UNNECESSARYENGINE_PARALLEL_GENERATION_H

#include <memory>
#include <unnecessary/jobs/worker_chain.h>
#include <unnecessary/serializer/generation_plan.h>
#include <unnecessary/serializer/jobs/generate_serializer_job.h>

namespace un {
    class ParallelGenerationPlan {
    private:
        std::shared_ptr<un::GenerationPlan> plan;
        std::vector<std::shared_ptr<un::Buffer>> buffers;
    public:
        void schedule(SimpleJobSystem& jobSystem) {
            un::WorkerChain<JobWorker> chain;
            plan->getIncludeGraph().each(
                [&](
                    u32 index,
                    const un::GenerationFile& vertex
                ) {
                    auto vec = vertex.getUnit().collectSymbols<un::CXXDeclaration>();
                    for (const auto& item : vec) {

                        const std::shared_ptr<un::Buffer>& buf = buffers.emplace_back(
                            std::make_shared<un::Buffer>()
                        );
                        un::JobHandle handle = chain.immediately<un::GenerateSerializerJob>(
                            buf,
                            item,
                            &vertex.getUnit()
                        );
                    }
                },
                [](
                    u32 from,
                    u32 to,
                    const un::DependencyType& edge
                ) {
                    if (edge != un::DependencyType::eUses) {
                        return;
                    }
                }
            );
        }
    };
}

#endif //UNNECESSARYENGINE_PARALLEL_GENERATION_H
