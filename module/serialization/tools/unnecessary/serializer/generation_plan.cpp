#include <utility>
#include <unnecessary/misc/pretty_print.h>
#include <unnecessary/serializer/generation_plan.h>

namespace un {
    generation_plan::generation_plan(
        std::shared_ptr<cppast::cpp_entity_index> index,
        std::filesystem::path output
    ) :
        _output(std::move(output)),
        _index(std::make_shared<cppast::cpp_entity_index>()) {
    }

    const std::shared_ptr<cppast::cpp_entity_index>& generation_plan::get_index() const {
        return _index;
    }

    std::ostream& operator<<(std::ostream& os, const un::generation_file& file) {
        os << uri(file._source);
        return os;
    }

    const std::filesystem::path& generation_file::get_output() const {
        return _output;
    }
}
