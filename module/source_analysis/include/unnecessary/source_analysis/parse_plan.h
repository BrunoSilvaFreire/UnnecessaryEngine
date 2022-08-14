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
    class ParseArguments {
    private:
        std::vector<std::filesystem::path> includes;
    public:
        const std::vector<std::filesystem::path>& getIncludes() const;

        friend class ParsePlan;
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
        std::filesystem::path _path;
        std::unique_ptr<cppast::cpp_file> _file;
        un::ParseReport _report;
    public:
        ParsedFile(
            std::unique_ptr<cppast::cpp_file>&& file,
            ParseReport report,
            std::filesystem::path path
        );

        const std::unique_ptr<cppast::cpp_file>& getFile() const;

        std::unique_ptr<cppast::cpp_file>& getFile();

        const ParseReport& getReport() const;

        const std::filesystem::path& getPath() const;
    };

    class ParsePlan {
    private:
        ParseArguments _arguments;
        std::shared_ptr<cppast::cpp_entity_index> _index;
        std::mutex _mutex;
        std::unordered_set<std::string> _alreadyParsed;
        std::unordered_map<std::string, std::unique_ptr<un::ParsedFile>> _parsed;

        void onParsed(
            std::unique_ptr<un::ParsedFile>& ptr,
            un::DynamicChain<SimpleJobSystem>& builder
        );

    public:
        explicit ParsePlan(std::shared_ptr<cppast::cpp_entity_index> index);

        void addInclude(const std::filesystem::path& include);


        std::unique_ptr<un::ParsedFile>& addFile(const std::filesystem::path& file);

        std::unique_ptr<un::ParsedFile>& addFile(const std::string& selfInclude, const std::filesystem::path& include) {
            return _parsed[selfInclude];
        }

        std::vector<un::CXXTranslationUnit> parse(un::ptr<un::SimpleJobSystem> jobSystem);
    };
}
#endif