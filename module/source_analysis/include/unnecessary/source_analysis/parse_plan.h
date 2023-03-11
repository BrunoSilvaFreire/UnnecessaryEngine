//
// Created by bruno on 27/06/2022.
//

#ifndef UNNECESSARYENGINE_PARSE_PLAN_H
#define UNNECESSARYENGINE_PARSE_PLAN_H

#include <unordered_set>
#include <mutex>
#include <utility>
#include <cppast/visitor.hpp>
#include <cppast/cpp_entity_index.hpp>
#include <cppast/libclang_parser.hpp>
#include <unnecessary/misc/benchmark.h>
#include <unnecessary/jobs/simple_jobs.h>
#include <unnecessary/jobs/dynamic_chain.h>
#include <unnecessary/source_analysis/cppast/transpiler.h>
#include <unnecessary/source_analysis/unnecessary_logger.h>

namespace un {
    class parse_arguments {
    private:
        std::vector<std::filesystem::path> _includes;

    public:
        const std::vector<std::filesystem::path>& get_includes() const;

        friend class parse_plan;
    };

    class parse_diagnostic {
    public:
        enum type {
            info,
            warn,
            error
        };

    private:
        type _type;
        std::string _message;

    public:
        parse_diagnostic(type type, std::string message);

        type get_type() const;

        const std::string& get_message() const;
    };

    class parse_report {
    private:
        std::chrono::milliseconds _parseStart;
        std::chrono::milliseconds _parseEnd;
        std::vector<parse_diagnostic> _diagnostics;

    public:
        const std::vector<parse_diagnostic>& get_diagnostics() const;

        std::chrono::milliseconds get_parse_duration() const;

        parse_report(
            std::chrono::milliseconds parseStart,
            std::chrono::milliseconds parseEnd,
            std::vector<parse_diagnostic> diagnostics
        );
    };

    class parsed_file {
    private:
        std::filesystem::path _path;
        std::unique_ptr<cppast::cpp_file> _file;
        parse_report _report;

    public:
        parsed_file(
            std::unique_ptr<cppast::cpp_file>&& file,
            parse_report report,
            std::filesystem::path path
        );

        const std::unique_ptr<cppast::cpp_file>& get_file() const;

        std::unique_ptr<cppast::cpp_file>& get_file();

        const parse_report& get_report() const;

        const std::filesystem::path& get_path() const;
    };

    class parse_plan {
    private:
        parse_arguments _arguments;
        std::shared_ptr<cppast::cpp_entity_index> _index;
        std::mutex _mutex;
        std::unordered_set<std::string> _alreadyParsed;
        std::unordered_map<std::string, std::unique_ptr<parsed_file>> _parsed;

        void on_parsed(
            std::unique_ptr<parsed_file>& ptr,
            dynamic_chain<simple_job_system>& builder
        );

    public:
        explicit parse_plan(std::shared_ptr<cppast::cpp_entity_index> index);

        void add_include(const std::filesystem::path& include);

        std::unique_ptr<parsed_file>& add_file(const std::filesystem::path& file);

        std::unique_ptr<parsed_file>&
        add_file(const std::string& selfInclude, const std::filesystem::path& include) {
            return _parsed[selfInclude];
        }

        std::vector<cxx_translation_unit> parse(ptr<simple_job_system> jobSystem);
    };
}
#endif
