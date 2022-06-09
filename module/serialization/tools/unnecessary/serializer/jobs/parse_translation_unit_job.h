//
// Created by brunorbsf on 29/05/22.
//

#ifndef UNNECESSARYENGINE_PARSE_TRANSLATION_UNIT_JOB_H
#define UNNECESSARYENGINE_PARSE_TRANSLATION_UNIT_JOB_H

#include <vector>
#include <filesystem>
#include <unnecessary/jobs/simple_jobs.h>
#include <unnecessary/serializer/generation_plan.h>

namespace un {

    class ParseTranslationUnitJob : public un::SimpleJob {
    private:
        std::filesystem::path file;
        std::filesystem::path relativeTo;
        std::filesystem::path debugOutput;
        std::vector<std::string> includes;
        un::GenerationPlan* plan;
        std::shared_ptr<cppast::cpp_entity_index> index;
    public:
        ParseTranslationUnitJob(
            std::shared_ptr<cppast::cpp_entity_index> index,
            std::filesystem::path  file,
            std::filesystem::path  relativeTo,
            std::filesystem::path  debugOutput,
            std::vector<std::string>  includes,
            GenerationPlan* plan
        );

        void operator()(WorkerType* worker) override;
    };
}


#endif //UNNECESSARYENGINE_PARSE_TRANSLATION_UNIT_JOB_H
