#include <unnecessary/serializer/jobs/generate_includes_job.h>

#include <utility>

namespace un {
    void generate_includes_jobs::operator()(job_worker* worker) {
        std::stringstream ss;
        std::string macroName = "UN_SERIALIZER_GENERATED_";
        std::filesystem::path loc = translationUnit->get_location();
        macroName += upper(loc.replace_extension().filename().string());
        ss << "// BEGIN GENERATED INCLUDES" << std::endl;
        ss << "#ifndef " << macroName << std::endl;
        ss << "#define " << macroName << std::endl;
        ss << "#include <unnecessary/serialization/serializer.h>" << std::endl;
        ss << "#include <unnecessary/serialization/serialization_utils.h>" << std::endl;
        ss << "// BEGIN TRANSLATION UNIT INCLUDES" << std::endl;
        for (const auto& item : translationUnit->get_includes()) {
            ss << "#include <" << item << ">" << std::endl;
        }
        //        const auto& includeGraph = plan->getIncludeGraph();
        //        for (u32 dependencyIndex : includeGraph.dependenciesOf(unit_graphIndex)) {
        //            const un::GenerationFile& file = *includeGraph[dependencyIndex];
        //            ss << "#include <" << file.getOutput().filename().string() << ">" << std::endl;
        //        }
        ss << " // END TRANSLATION UNIT INCLUDES" << std::endl;
        ss << "// Self include" << std::endl;
        ss << "#include <" << translationUnit->get_self_include() << ">" << std::endl;
        ss << "#endif" << std::endl;
        ss << " // END GENERATED INCLUDES" << std::endl;
        buf->operator=(ss.str());
    }

    generate_includes_jobs::generate_includes_jobs(
        u32 unitGraphIndex,
        const_ptr<cxx_translation_unit> translationUnit,
        const_ptr<generation_plan> plan,
        std::shared_ptr<byte_buffer> buf
    ) : translationUnit(translationUnit),
        plan(plan), unit_graphIndex(unitGraphIndex), buf(std::move(buf)) {
    }
}
