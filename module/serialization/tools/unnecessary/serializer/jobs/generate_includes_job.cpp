#include <unnecessary/serializer/jobs/generate_includes_job.h>

#include <utility>

namespace un {
    void GenerateIncludesJobs::operator()(JobWorker* worker) {
        std::stringstream ss;
        std::string macroName = "UN_SERIALIZER_GENERATED_";
        std::filesystem::path loc = translationUnit->getLocation();
        macroName += un::upper(loc.replace_extension().filename().string());
        ss << "#ifndef " << macroName << std::endl;
        ss << "#define " << macroName << std::endl;

        for (const auto& item : translationUnit->getIncludes()) {
            ss << "#include <" << item << ">" << std::endl;
        }
        const auto& includeGraph = plan->getIncludeGraph();
        for (u32 dependencyIndex : includeGraph.dependenciesOf(unitGraphIndex)) {
            const un::GenerationFile& file = *includeGraph[dependencyIndex];
            ss << "#include <" << file.getOutput().filename().string() << ">" << std::endl;
        }
        ss << "#include <" << translationUnit->getSelfInclude() << ">" << std::endl;
        ss << "#endif" << std::endl;
        buf->operator=(ss.str());
    }

    GenerateIncludesJobs::GenerateIncludesJobs(
        u32 unitGraphIndex,
        un::const_ptr<CXXTranslationUnit> translationUnit,
        un::const_ptr<un::GenerationPlan> plan,
        std::shared_ptr<un::Buffer> buf
    ) : translationUnit(translationUnit),
        buf(std::move(buf)), plan(plan), unitGraphIndex(unitGraphIndex) { }
}
