#include <unnecessary/misc/files.h>
#include <unnecessary/source_analysis/parser.h>
#include <unnecessary/source_analysis/parsing.h>
#include <unnecessary/serializer/jobs/parse_translation_unit_job.h>

namespace un {
    void ParseTranslationUnitJob::operator()(JobWorker* worker) {
        std::filesystem::path path = file;
        std::filesystem::path relative = std::filesystem::relative(file, relativeTo);
        un::parsing::ParsingOptions parseOptions(file, includes);
        std::string debugFileName;
        debugFileName += path.filename().string();
        debugFileName += ".info.txt";
        std::filesystem::path debugFile = debugOutput / debugFileName;
        un::files::ensure_directory_exists(debugFile.parent_path());
        parseOptions.setDebugFile(debugFile);
        parseOptions.setSelfInclude(un::to_string(relative));
        auto translationUnit = un::parsing::parse(parseOptions);
        plan->addTranslationUnit(relative, std::move(translationUnit));
    }

    ParseTranslationUnitJob::ParseTranslationUnitJob(
        const std::filesystem::path& file,
        const std::filesystem::path& relativeTo,
        const std::filesystem::path& debugOutput,
        const std::vector<std::string>& includes,
        GenerationPlan* plan
    ) : file(file), relativeTo(relativeTo), debugOutput(debugOutput), includes(includes), plan(plan) { }
}

