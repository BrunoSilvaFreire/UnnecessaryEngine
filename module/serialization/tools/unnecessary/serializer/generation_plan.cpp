//
// Created by bruno on 22/05/2022.
//

#include <unnecessary/serializer/generation_plan.h>

namespace un {

    GenerationPlan::GenerationPlan(const std::filesystem::path& source)
        : source(source), include2Index(), includeGraph() { }
}
