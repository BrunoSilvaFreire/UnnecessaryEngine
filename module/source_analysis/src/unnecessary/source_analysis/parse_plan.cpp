#include <utility>
#include <cppast/visitor.hpp>
#include <unnecessary/source_analysis/parse_plan.h>
#include <unnecessary/source_analysis/jobs/parse_file_job.h>

namespace un {

    const std::vector<std::filesystem::path>& ParseArguments::getIncludes() const {
        return includes;
    }

    ParsePlan::ParsePlan(
        ParseArguments arguments,
        un::ptr<SimpleJobSystem> jobSystem
    ) : _arguments(std::move(arguments)), _jobSystem(jobSystem) {
    }

    bool ParsePlan::tryAccess(const std::string& include, JobHandle& handle) {
        std::lock_guard<std::mutex> lock(_mutex);
        if (_alreadyParsed.contains(include)) {
            return false;
        }

        return true;
    }

    const std::unique_ptr<cppast::cpp_file>& ParsePlan::operator()(
        const std::filesystem::path& file,
        cppast::cpp_entity_index& index
    ) {
        un::UnnecessaryLogger logger;
        cppast::libclang_parser parser(type_safe::ref(logger));
        cppast::libclang_compile_config config;
        config.set_flags(cppast::cpp_standard::cpp_20);
        for (const std::filesystem::path& item : _arguments.getIncludes()) {
            config.add_include_dir(item.string());
        }
        const std::string& path = file.string();
        std::unique_ptr<un::ParsedFile>& ptr = _parsed[path];
        auto chain = un::create_dynamic_chain(
            _jobSystem,
            [this, &index, path]<typename TArch>(
                un::JobHandle handle,
                un::SimpleJob* job,
                un::JobChain<un::SimpleJobSystem>& subChain
            ) {
                std::unique_ptr<un::ParsedFile>& ptr = _parsed[path];
                const std::unique_ptr<cppast::cpp_file>& uniquePtr = ptr->getFile();
                cppast::visit(
                    *uniquePtr,
                    [](const cppast::cpp_entity& e) {
                        return e.kind() == cppast::cpp_entity_kind::include_directive_t;
                    },
                    [&](const cppast::cpp_entity& e, cppast::visitor_info info) {
                        const auto& includeDirective = dynamic_cast<const cppast::cpp_include_directive&>(e);
                        auto iKind = includeDirective.include_kind();
                        if (iKind != cppast::cpp_include_kind::local) {
                            return;
                        }
                        auto includePath = includeDirective.full_path();
                        if (_parsed.contains(includePath)) {
                            return;
                        }
                        subChain.immediately<un::ParseFileJob>(
                            includePath,
                            &index,
                            &ptr,
                            _arguments
                        );
                    }
                );
            }
        );
        chain.include<un::ParseFileJob>(
            file,
            &index,
            &ptr,
            _arguments
        );

        chain.wait();
        return ptr->getFile();
    }

    ParseDiagnostic::ParseDiagnostic(
        ParseDiagnostic::Type type,
        std::string message
    ) : _type(type), _message(std::move(message)) { }

    ParseDiagnostic::Type ParseDiagnostic::getType() const {
        return _type;
    }

    const std::string& ParseDiagnostic::getMessage() const {
        return _message;
    }

    const std::vector<un::ParseDiagnostic>& ParseReport::getDiagnostics() const {
        return _diagnostics;
    }

    ParseReport::ParseReport(
        std::chrono::milliseconds parseStart,
        std::chrono::milliseconds parseEnd,
        std::vector<un::ParseDiagnostic> diagnostics
    ) : _parseStart(parseStart), _parseEnd(parseEnd), _diagnostics(std::move(diagnostics)) { }

    std::chrono::milliseconds ParseReport::getParseDuration() const {
        return _parseEnd - _parseStart;
    }

    ParsedFile::ParsedFile(
        std::unique_ptr<cppast::cpp_file>&& file,
        const ParseReport& report
    ) : _file(std::move(file)),
        _report(report) { }

    const std::unique_ptr<cppast::cpp_file>& ParsedFile::getFile() const {
        return _file;
    }

    const ParseReport& ParsedFile::getReport() const {
        return _report;
    }
}