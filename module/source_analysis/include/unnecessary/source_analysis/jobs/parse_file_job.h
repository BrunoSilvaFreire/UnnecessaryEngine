#ifndef UNNECESSARYENGINE_PARSE_FILE_JOB_H
#define UNNECESSARYENGINE_PARSE_FILE_JOB_H

#include <unnecessary/jobs/simple_jobs.h>
#include <unnecessary/source_analysis/parse_plan.h>
#include <unnecessary/source_analysis/unnecessary_logger.h>
#include <cppast/libclang_parser.hpp>
#include <utility>

namespace un {
    class ParseFileJob : public un::SimpleJob {
    private:
        std::filesystem::path _file;
        un::const_ptr<cppast::cpp_entity_index> _index;
        un::ptr<std::unique_ptr<un::ParsedFile>> _output;
        un::ParseArguments _arguments;
    public:
        ParseFileJob(
            std::filesystem::path file,
            un::const_ptr<cppast::cpp_entity_index> index,
            un::ptr<std::unique_ptr<un::ParsedFile>> output,
            ParseArguments arguments
        );

        ~ParseFileJob() override;

        void operator()(WorkerType* worker) override;

        const std::filesystem::path& getFile() const;

        std::unique_ptr<un::ParsedFile>* getOutput();
    };
}
#endif