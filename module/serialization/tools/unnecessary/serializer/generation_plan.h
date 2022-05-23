//
// Created by bruno on 22/05/2022.
//

#ifndef UNNECESSARYENGINE_GENERATION_PLAN_H
#define UNNECESSARYENGINE_GENERATION_PLAN_H

#include <filesystem>
#include <cppast/libclang_parser.hpp>
#include <cppast/cpp_file.hpp>
#include <grapphs/algorithms/rlo_traversal.h>
#include <unnecessary/graphs/dependency_graph.h>
#include <unnecessary/source_analysis/parser.h>

namespace un {
    class GenerationFile {
    private:
        std::filesystem::path path;
        un::CXXTranslationUnit unit;
    public:
        GenerationFile(
            const std::filesystem::path& path,
            const un::parsing::ParsingOptions& options
        ) {

        }
    };

    class GenerationPlan {
    public:
        using IncludeGraphType = un::DependencyGraph<un::GenerationFile>;
    private:
        std::filesystem::path source;
        std::unordered_map<std::string, u32> include2Index;
        IncludeGraphType includeGraph;
    public:
        explicit GenerationPlan(const std::filesystem::path& source);

        void addTranslationUnit(
            const std::filesystem::path& file,
            un::CXXTranslationUnit&& unit
        ) {

        }

        std::vector<std::pair<u32, const un::GenerationFile*>> getFilesSequence() const {
            return includeGraph.get_rlo_sequence();
        }

    };
}

#endif //UNNECESSARYENGINE_GENERATION_PLAN_H
