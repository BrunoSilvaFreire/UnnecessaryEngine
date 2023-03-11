#include <unnecessary/source_analysis/jobs/parse_file_job.h>
#include <unnecessary/misc/pretty_print.h>

namespace un {
    void parse_file_job::operator()(job_worker* worker) {
        static unnecessary_logger logger;
        cppast::libclang_parser parser(type_safe::ref(logger));
        cppast::libclang_compile_config config;
        config.set_flags(cppast::cpp_standard::cpp_20);
        for (const std::filesystem::path& item : _arguments.get_includes()) {
            config.add_include_dir(item.string());
        }

        chronometer<> chronometer;
        std::vector<parse_diagnostic> diagnostics;
        std::unique_ptr<cppast::cpp_file> parsed = parser.parse(_index, _file.string(), config);
        std::chrono::milliseconds duration = chronometer.stop();
        auto start = chronometer.get_start();
        auto end = start + duration;
        parse_report report(
            std::chrono::duration_cast<std::chrono::milliseconds>(start.time_since_epoch()),
            std::chrono::duration_cast<std::chrono::milliseconds>(end.time_since_epoch()),
            diagnostics
        );
        auto pFile = std::make_unique<parsed_file>(
            std::move(parsed),
            report,
            _file
        );
        _output = std::move(pFile);
    }

    parse_file_job::parse_file_job(
        std::filesystem::path file,
        const cppast::cpp_entity_index& index,
        ref<std::unique_ptr<parsed_file>> output,
        parse_arguments arguments
    ) : _file(std::move(file)),
        _index(index),
        _output(output),
        _arguments(std::move(arguments)) {
        std::stringstream ss;
        ss << "Parse '" << un::uri(_file) << "'";
        set_name(ss.str());
    }

    const std::filesystem::path& parse_file_job::get_file() const {
        return _file;
    }

    std::unique_ptr<parsed_file>& parse_file_job::get_output() {
        return _output;
    }
}
