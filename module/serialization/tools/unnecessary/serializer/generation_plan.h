#ifndef UNNECESSARYENGINE_GENERATION_PLAN_H
#define UNNECESSARYENGINE_GENERATION_PLAN_H

#include <filesystem>
#include <cppast/libclang_parser.hpp>
#include <cppast/cpp_file.hpp>
#include <utility>
#include <grapphs/algorithms/rlo_traversal.h>
#include <unnecessary/graphs/dependency_graph.h>
#include <unnecessary/source_analysis/structures.h>
#include <ostream>

namespace un {
    class generation_file {
    private:
        std::filesystem::path _source;
        std::filesystem::path _output;
        cxx_translation_unit _unit;

    public:
        generation_file(
            std::filesystem::path path,
            std::filesystem::path output,
            cxx_translation_unit&& unit
        ) : _source(std::move(path)),
            _output(std::move(output)),
            _unit(std::move(unit)) {
        }

        const std::filesystem::path& get_source() const {
            return _source;
        }

        const cxx_translation_unit& get_unit() const {
            return _unit;
        }

        const std::filesystem::path& get_output() const;

        friend std::ostream& operator<<(std::ostream& os, const generation_file& file);
    };

    class generation_plan {
    private:
        std::filesystem::path _source;
        std::filesystem::path _output;
        std::shared_ptr<cppast::cpp_entity_index> _index;

    public:
        explicit generation_plan(
            std::shared_ptr<cppast::cpp_entity_index> index,
            std::filesystem::path output
        );

        const std::shared_ptr<cppast::cpp_entity_index>& get_index() const;
    };
}

#endif //UNNECESSARYENGINE_GENERATION_PLAN_H
