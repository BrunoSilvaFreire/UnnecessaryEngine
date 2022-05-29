//
// Created by bruno on 22/05/2022.
//

#include <unnecessary/serializer/generation_plan.h>

namespace un {

    GenerationPlan::GenerationPlan(const std::filesystem::path& source)
        : source(source), include2Index(), includeGraph() { }

    const GenerationPlan::IncludeGraphType& GenerationPlan::getIncludeGraph() const {
        return includeGraph;
    }

    void GenerationPlan::addTranslationUnit(const std::filesystem::path& file, CXXTranslationUnit&& unit) {
        auto i = includeGraph.add(un::GenerationFile(file, std::move(unit)));
        include2Index[un::to_string(file)] = i;
    }

    void GenerationPlan::bake() {
        for (const auto& [gen, i] : includeGraph.getInnerGraph().all_vertices()) {
            for (const auto& item : gen->getUnit().getIncludes()) {
                auto it = include2Index.find(item);
                if (it != include2Index.end()) {
                    includeGraph.addDependency(i, it->second);
                }
            }
        }
    }

    std::vector<std::pair<u32, const un::GenerationFile*>> GenerationPlan::getFilesSequence() const {
        return includeGraph.get_rlo_sequence();
    }

    std::ostream& operator<<(std::ostream& os, const GenerationFile& file) {
        os << un::to_string(file.path);
        return os;
    }
}
