#ifndef UNNECESSARYENGINE_PARSE_FILE_JOB_H
#define UNNECESSARYENGINE_PARSE_FILE_JOB_H

#include <unnecessary/jobs/simple_jobs.h>
#include <unnecessary/source_analysis/parse_plan.h>
#include <unnecessary/source_analysis/unnecessary_logger.h>
#include <cppast/libclang_parser.hpp>
#include <utility>

namespace un {
    class parse_file_job : public simple_job {
    private:
        std::filesystem::path _file;
        const_ref<cppast::cpp_entity_index> _index;
        ref<std::unique_ptr<parsed_file>> _output;
        parse_arguments _arguments;

    public:
        parse_file_job(
            std::filesystem::path file,
            const_ref<cppast::cpp_entity_index> index,
            ref<std::unique_ptr<parsed_file>> output,
            parse_arguments arguments
        );

        void operator()(worker_type* worker) override;

        const std::filesystem::path& get_file() const;

        std::unique_ptr<parsed_file>& get_output();
    };
}
#endif
