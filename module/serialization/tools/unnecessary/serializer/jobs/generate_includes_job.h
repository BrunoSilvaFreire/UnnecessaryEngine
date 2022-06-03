//
// Created by bruno on 03/06/2022.
//

#ifndef UNNECESSARYENGINE_GENERATE_INCLUDES_JOB_H
#define UNNECESSARYENGINE_GENERATE_INCLUDES_JOB_H

#include <unnecessary/memory/membuf.h>
#include <unnecessary/jobs/simple_jobs.h>
#include <unnecessary/source_analysis/structures.h>
#include <unnecessary/serializer/generation_plan.h>

namespace un {
    class GenerateIncludesJobs : public SimpleJob {
    private:
        un::const_ptr<un::CXXTranslationUnit> translationUnit;
        un::const_ptr<un::GenerationPlan> plan;
        u32 unitGraphIndex;
        std::shared_ptr<un::Buffer> buf;
    public:
        GenerateIncludesJobs(
            u32 unitGraphIndex,
            un::const_ptr<CXXTranslationUnit> translationUnit,
            un::const_ptr<un::GenerationPlan> plan,
            std::shared_ptr<un::Buffer> buf
        );

        void operator()(WorkerType* worker) override;
    };
}


#endif //UNNECESSARYENGINE_GENERATE_INCLUDES_JOB_H
