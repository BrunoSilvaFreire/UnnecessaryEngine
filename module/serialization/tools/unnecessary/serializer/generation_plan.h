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
    class GenerationFile {
    private:
        std::filesystem::path source;
        std::filesystem::path output;
        un::CXXTranslationUnit unit;
    public:
        GenerationFile(
            std::filesystem::path path,
            std::filesystem::path output,
            CXXTranslationUnit&& unit
        ) : source(std::move(path)),
            unit(std::move(unit)),
            output(std::move(output)) { }

        const std::filesystem::path& getSource() const {
            return source;
        }

        const CXXTranslationUnit& getUnit() const {
            return unit;
        }

        const std::filesystem::path& getOutput() const;

        friend std::ostream& operator<<(std::ostream& os, const GenerationFile& file);
    };

    class GenerationPlan {
    private:
        std::filesystem::path source;
        std::filesystem::path output;
        std::shared_ptr<cppast::cpp_entity_index> index;

    public:
        explicit GenerationPlan(
            std::shared_ptr<cppast::cpp_entity_index> index,
            std::filesystem::path output
        );

        const std::shared_ptr<cppast::cpp_entity_index>& getIndex() const;
    };
}

#endif //UNNECESSARYENGINE_GENERATION_PLAN_H
