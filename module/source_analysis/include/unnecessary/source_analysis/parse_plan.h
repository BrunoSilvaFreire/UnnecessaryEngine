//
// Created by bruno on 27/06/2022.
//

#ifndef UNNECESSARYENGINE_PARSE_PLAN_H
#define UNNECESSARYENGINE_PARSE_PLAN_H

#include <unordered_set>
#include <mutex>
#include <utility>
#include <cppast/cpp_entity_index.hpp>
#include <cppast/libclang_parser.hpp>
#include <unnecessary/misc/benchmark.h>
#include <unnecessary/jobs/simple_jobs.h>
#include <unnecessary/jobs/dynamic_chain.h>
#include <unnecessary/source_analysis/unnecessary_logger.h>

namespace un {
    class ParseArguments {
    private:
        std::vector<std::filesystem::path> includes;
    public:
        const std::vector<std::filesystem::path>& getIncludes() const;

        void addInclude(const std::filesystem::path& include) {
            includes.emplace_back(include);
        }
    };

    class ParseDiagnostic {
    public:
        enum Type {
            eInfo,
            eWarn,
            eError
        };
    private:
        Type _type;
        std::string _message;
    public:
        ParseDiagnostic(Type type, std::string message);

        Type getType() const;

        const std::string& getMessage() const;
    };


    class ParseReport {
    private:
        std::chrono::milliseconds _parseStart;
        std::chrono::milliseconds _parseEnd;
        std::vector<un::ParseDiagnostic> _diagnostics;
    public:

        const std::vector<un::ParseDiagnostic>& getDiagnostics() const;

        std::chrono::milliseconds getParseDuration() const;

        ParseReport(
            std::chrono::milliseconds parseStart,
            std::chrono::milliseconds parseEnd,
            std::vector<un::ParseDiagnostic> diagnostics
        );
    };

    class ParsedFile {
    private:
        std::unique_ptr<cppast::cpp_file> _file;
        un::ParseReport _report;
    public:
        ParsedFile(
            std::unique_ptr<cppast::cpp_file>&& file,
            const ParseReport& report
        );

        const std::unique_ptr<cppast::cpp_file>& getFile() const;

        const ParseReport& getReport() const;
    };

    class ParsePlan {
    private:
        ParseArguments _arguments;
        std::mutex _mutex;
        std::unordered_set<std::string> _alreadyParsed;
        std::unordered_map<std::string, std::unique_ptr<un::ParsedFile>> _parsed;
        un::ptr<un::SimpleJobSystem> _jobSystem;
    public:
        ParsePlan(
            ParseArguments arguments,
            un::ptr<SimpleJobSystem> jobSystem
        );

        bool tryAccess(const std::string& include, un::JobHandle& handle);

        const std::unique_ptr<cppast::cpp_file>& operator()(
            const std::filesystem::path& file,
            cppast::cpp_entity_index& index
        );
    };
}
#endif