#include <unnecessary/misc/files.h>
#include <unnecessary/source_analysis/parser.h>
#include <unnecessary/source_analysis/parsing.h>
#include <unnecessary/serializer/jobs/parse_translation_unit_job.h>

#include <utility>

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
        parseOptions.setIndexToUse(index);
        auto translationUnit = un::parsing::parse(parseOptions);
        plan->addTranslationUnit(relative, std::move(translationUnit));
    }

    ParseTranslationUnitJob::ParseTranslationUnitJob(
        std::shared_ptr<cppast::cpp_entity_index> index,
        std::filesystem::path file,
        std::filesystem::path relativeTo,
        std::filesystem::path debugOutput,
        std::vector<std::string> includes,
        GenerationPlan* plan
    ) : file(std::move(file)),
        relativeTo(std::move(relativeTo)),
        debugOutput(std::move(debugOutput)),
        includes(std::move(includes)),
        plan(plan),
        index(std::move(index)) { }
}

