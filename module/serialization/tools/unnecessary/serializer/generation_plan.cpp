#include <utility>
#include <unnecessary/misc/pretty_print.h>
#include <unnecessary/serializer/generation_plan.h>

namespace un {

    GenerationPlan::GenerationPlan(
        std::shared_ptr<cppast::cpp_entity_index> index,
        std::filesystem::path output
    ) :
        output(std::move(output)),
        index(std::make_shared<cppast::cpp_entity_index>()) {
    }

    const std::shared_ptr<cppast::cpp_entity_index>& GenerationPlan::getIndex() const {
        return index;
    }

    std::ostream& operator<<(std::ostream& os, const GenerationFile& file) {
        os << un::prettify(file.source);
        return os;
    }

    const std::filesystem::path& GenerationFile::getOutput() const {
        return output;
    }
}
