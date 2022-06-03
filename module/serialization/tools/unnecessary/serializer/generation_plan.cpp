//
// Created by bruno on 22/05/2022.
//

#include <unnecessary/serializer/generation_plan.h>

namespace un {

    GenerationPlan::GenerationPlan(
        const std::filesystem::path& source,
        std::filesystem::path output
    )
        : source(source), include2Index(), includeGraph(), output(output) { }

    const GenerationPlan::IncludeGraphType& GenerationPlan::getIncludeGraph() const {
        return includeGraph;
    }

    void GenerationPlan::addTranslationUnit(const std::filesystem::path& file, CXXTranslationUnit&& unit) {
        {
            std::lock_guard<decltype(translationUnitMutexes)> lock(translationUnitMutexes);
            std::filesystem::path fileName = file.filename();;
            fileName.replace_extension();
            std::string finalName = fileName.filename().string() + ".serializer.generated.h";
            auto i = includeGraph.add(
                un::GenerationFile(
                    file,
                    output / "include" / finalName,
                    std::move(unit)
                )
            );
            include2Index[un::to_string(file)] = i;
        }
    }

    void GenerationPlan::bake() {
        {
            std::lock_guard<decltype(translationUnitMutexes)> lock(translationUnitMutexes);
            for (const auto& [gen, i] : includeGraph.getInnerGraph().all_vertices()) {
                for (const auto& item : gen->getUnit().getIncludes()) {
                    auto it = include2Index.find(item);
                    if (it != include2Index.end()) {
                        includeGraph.addDependency(i, it->second);
                    }
                }
            }
        }
    }

    std::vector<std::pair<u32, const un::GenerationFile*>> GenerationPlan::getFilesSequence() const {
        return includeGraph.get_rlo_sequence();
    }

    GenerationPlan::IncludeGraphType& GenerationPlan::getIncludeGraph() {
        return includeGraph;
    }

    std::ostream& operator<<(std::ostream& os, const GenerationFile& file) {
        os << un::to_string(file.path);
        return os;
    }

    const std::filesystem::path& GenerationFile::getOutput() const {
        return output;
    }
}
