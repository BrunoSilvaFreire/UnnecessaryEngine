#include <unnecessary/source_analysis/jobs/parse_file_job.h>

namespace un {

    void ParseFileJob::operator()(JobWorker* worker) {
        static un::UnnecessaryLogger logger;
        cppast::libclang_parser parser(type_safe::ref(logger));
        cppast::libclang_compile_config config;
        config.set_flags(cppast::cpp_standard::cpp_20);
        for (const std::filesystem::path& item : _arguments.getIncludes()) {
            config.add_include_dir(item.string());
        }

        un::Chronometer<> chronometer;
        std::vector<un::ParseDiagnostic> diagnostics;
        std::unique_ptr<cppast::cpp_file> parsed = parser.parse(*_index, _file.string(), config);
        std::chrono::milliseconds duration = chronometer.stop();
        auto start = chronometer.getStart();
        auto end = start + duration;
        un::ParseReport report(
            std::chrono::duration_cast<std::chrono::milliseconds>(start.time_since_epoch()),
            std::chrono::duration_cast<std::chrono::milliseconds>(end.time_since_epoch()),
            diagnostics
        );
        std::unique_ptr<un::ParsedFile> pFile = std::make_unique<un::ParsedFile>(
            std::move(parsed),
            report
        );
        _output->operator=(std::move(pFile));
    }

    ParseFileJob::ParseFileJob(
        std::filesystem::path file,
        un::const_ptr<cppast::cpp_entity_index> index,
        un::ptr<std::unique_ptr<un::ParsedFile>> output,
        ParseArguments arguments
    ) : _file(std::move(file)),
        _index(index),
        _output(output),
        _arguments(std::move(arguments)) { }
}