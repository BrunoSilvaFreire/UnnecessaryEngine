#include <unnecessary/source_analysis/parse_plan.h>
#include <unnecessary/source_analysis/jobs/parse_file_job.h>
#include <algorithm>
#include <utility>

namespace un {
    const std::vector<std::filesystem::path>& parse_arguments::get_includes() const {
        return _includes;
    }

    parse_diagnostic::parse_diagnostic(
        type type,
        std::string message
    ) : _type(type), _message(std::move(message)) {
    }

    parse_diagnostic::type parse_diagnostic::get_type() const {
        return _type;
    }

    const std::string& parse_diagnostic::get_message() const {
        return _message;
    }

    const std::vector<parse_diagnostic>& parse_report::get_diagnostics() const {
        return _diagnostics;
    }

    parse_report::parse_report(
        std::chrono::milliseconds parseStart,
        std::chrono::milliseconds parseEnd,
        std::vector<parse_diagnostic> diagnostics
    ) : _parseStart(parseStart), _parseEnd(parseEnd), _diagnostics(std::move(diagnostics)) {
    }

    std::chrono::milliseconds parse_report::get_parse_duration() const {
        return _parseEnd - _parseStart;
    }

    parsed_file::parsed_file(
        std::unique_ptr<cppast::cpp_file>&& file,
        parse_report report,
        std::filesystem::path path
    ) : _path(std::move(path)),
        _file(std::move(file)), _report(std::move(report)) {
    }

    parse_plan::parse_plan(
        std::shared_ptr<cppast::cpp_entity_index> index
    ) : _index(std::move(index)) {
    }

    const std::unique_ptr<cppast::cpp_file>& parsed_file::get_file() const {
        return _file;
    }

    std::unique_ptr<cppast::cpp_file>& parsed_file::get_file() {
        return _file;
    }

    const parse_report& parsed_file::get_report() const {
        return _report;
    }

    const std::filesystem::path& parsed_file::get_path() const {
        return _path;
    }

    std::unique_ptr<parsed_file>& parse_plan::add_file(const std::filesystem::path& file) {
        return add_file(file.string(), file);
    }

    void parse_plan::on_parsed(
        std::unique_ptr<parsed_file>& ptr,
        dynamic_chain<simple_job_system>& builder
    ) {
        const std::filesystem::path& path = ptr->get_path();
        if (ptr == nullptr) {
            std::stringstream ss;
            ss << "Unable to find parsed file for path \"" << path << "\"";
            throw std::runtime_error(ss.str());
        }
        const std::unique_ptr<cppast::cpp_file>& uniquePtr = ptr->get_file();
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
                builder.enqueue(
                    [&](dynamic_chain<simple_job_system>& chain) {
                        on_parsed(includePtr, chain);
                    },
                    new parse_file_job(
                        includePath,
                        *_index,
                        includePtr,
                        _arguments
                    )
                );
            }
        );
    }

    std::vector<cxx_translation_unit> parse_plan::parse(ptr<simple_job_system> jobSystem) {
        unnecessary_logger logger;
        cppast::libclang_parser parser(type_safe::ref(logger));
        cppast::libclang_compile_config config;
        config.set_flags(cppast::cpp_standard::cpp_20);
        for (const std::filesystem::path& item : _arguments.get_includes()) {
            config.add_include_dir(item.string());
        }
        dynamic_flow<simple_job_system> flow(jobSystem);
        for (auto& pair : _parsed) {
            auto& key = pair.first;
            auto& value = pair.second;
            flow.enqueue<parse_file_job>(
                [&value, this](dynamic_chain<simple_job_system>& chain) {
                    on_parsed(value, chain);
                },
                new parse_file_job(
                    key,
                    *_index,
                    value,
                    _arguments
                )
            );
        }
        flow.wait();
        std::vector<cxx_translation_unit> translationUnits;
        for (const auto& item : this->_parsed) {
            const std::unique_ptr<parsed_file>& parsedFile = item.second;

            auto unit = &translationUnits.emplace_back(parsedFile->get_path(), item.first);
            std::unique_ptr<cppast::cpp_file>& pFile = parsedFile->get_file();

            transpiler transpiler(unit, _index);
            transpiler.parse(*pFile);
        }
        return translationUnits;
    }

    void parse_plan::add_include(const std::filesystem::path& include) {
        _arguments._includes.emplace_back(include);
    }
}
