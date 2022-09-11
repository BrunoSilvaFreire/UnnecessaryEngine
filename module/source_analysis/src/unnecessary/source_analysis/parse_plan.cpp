#include <unnecessary/source_analysis/parse_plan.h>
#include <unnecessary/source_analysis/jobs/parse_file_job.h>
#include <algorithm>
#include <utility>

namespace un {

    const std::vector<std::filesystem::path>& ParseArguments::getIncludes() const {
        return includes;
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
        ParseReport report,
        std::filesystem::path path
    ) : _file(std::move(file)),
        _report(std::move(report)), _path(std::move(path)) { }

    ParsePlan::ParsePlan(
        std::shared_ptr<cppast::cpp_entity_index> index
    ) : _index(std::move(index)) { }

    const std::unique_ptr<cppast::cpp_file>& ParsedFile::getFile() const {
        return _file;
    }


    std::unique_ptr<cppast::cpp_file>& ParsedFile::getFile() {
        return _file;
    }

    const ParseReport& ParsedFile::getReport() const {
        return _report;
    }

    const std::filesystem::path& ParsedFile::getPath() const {
        return _path;
    }

    std::unique_ptr<un::ParsedFile>& ParsePlan::addFile(const std::filesystem::path& file) {
        return addFile(file.string(), file);
    }

    void ParsePlan::onParsed(
        std::unique_ptr<un::ParsedFile>& ptr,
        un::DynamicChain<SimpleJobSystem>& builder
    ) {
        const std::filesystem::path& path = ptr->getPath();
        if (ptr == nullptr) {
            std::stringstream ss;
            ss << "Unable to find parsed file for path \"" << path << "\"";
            throw std::runtime_error(ss.str());
        }
        const std::unique_ptr<cppast::cpp_file>& uniquePtr = ptr->getFile();
        if (uniquePtr == nullptr) {
            std::stringstream ss;
            ss << "Found parsed file for path \"" << path << "\", but it's wrapped value is null.";
            throw std::runtime_error(ss.str());
        }
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
                auto& includePtr = _parsed.operator[](includePath);
                LOG(INFO) << "Sub @ " << includePath;
                builder.enqueue<un::ParseFileJob>(
                    [&](un::DynamicChain<un::SimpleJobSystem>& chain) {
                        onParsed(includePtr, chain);
                    },
                    includePath,
                    _index.get(),
                    &includePtr,
                    _arguments
                );
            }
        );
    }


    std::vector<un::CXXTranslationUnit> ParsePlan::parse(un::ptr<un::SimpleJobSystem> jobSystem) {
        un::UnnecessaryLogger logger;
        cppast::libclang_parser parser(type_safe::ref(logger));
        cppast::libclang_compile_config config;
        config.set_flags(cppast::cpp_standard::cpp_20);
        for (const std::filesystem::path& item : _arguments.getIncludes()) {
            config.add_include_dir(item.string());
        }
        un::DynamicFlow<un::SimpleJobSystem> flow(jobSystem);
        for (auto& pair : _parsed) {
            // enqueue(chain, key, value);
            auto& key = pair.first;
            auto& value = pair.second;
            flow.enqueue<un::ParseFileJob>(
                [&](un::DynamicChain<un::SimpleJobSystem>& chain) {
                    onParsed(value, chain);
                },
                key,
                _index.get(),
                &value,
                _arguments
            );
        }
        flow.wait();
        std::vector<un::CXXTranslationUnit> translationUnits;
        for (const auto& item : this->_parsed) {
            const std::unique_ptr<un::ParsedFile>& parsedFile = item.second;
            auto unit = &translationUnits.emplace_back(
                parsedFile->getPath(),
                item.first
            );
            std::unique_ptr<cppast::cpp_file>& pFile = parsedFile->getFile();
            un::Transpiler transpiler(
                unit,
                _index
            );
            transpiler.parse(*pFile);
        }
        return translationUnits;
    }

    void ParsePlan::addInclude(const std::filesystem::path& include) {
        _arguments.includes.emplace_back(include);
    }
}