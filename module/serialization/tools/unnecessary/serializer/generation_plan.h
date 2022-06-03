//
// Created by bruno on 22/05/2022.
//

#ifndef UNNECESSARYENGINE_GENERATION_PLAN_H
#define UNNECESSARYENGINE_GENERATION_PLAN_H

#include <filesystem>
#include <cppast/libclang_parser.hpp>
#include <cppast/cpp_file.hpp>
#include <utility>
#include <grapphs/algorithms/rlo_traversal.h>
#include <unnecessary/graphs/dependency_graph.h>
#include <unnecessary/source_analysis/parser.h>
#include <ostream>

namespace un {
    class GenerationFile {
    private:

        std::filesystem::path path;
        std::filesystem::path output;
        un::CXXTranslationUnit unit;
    public:
        GenerationFile(
            std::filesystem::path path,
            std::filesystem::path output,
            CXXTranslationUnit&& unit
        ) : path(std::move(path)), unit(std::move(unit)), output(output) { }

        const std::filesystem::path& getPath() const {
            return path;
        }

        const CXXTranslationUnit& getUnit() const {
            return unit;
        }

        const std::filesystem::path& getOutput() const;

        friend std::ostream& operator<<(std::ostream& os, const GenerationFile& file);
    };

    class GenerationPlan {
    public:
        using IncludeGraphType = un::DependencyGraph<un::GenerationFile>;
    private:
        std::filesystem::path source;
        std::filesystem::path output;
        std::unordered_map<std::string, u32> include2Index;
        IncludeGraphType includeGraph;
        std::mutex translationUnitMutexes;
    public:
        explicit GenerationPlan(
            const std::filesystem::path& source,
            std::filesystem::path output
        );

        void addTranslationUnit(
            const std::filesystem::path& file,
            un::CXXTranslationUnit&& unit
        );

        void bake();

        std::vector<std::pair<u32, const un::GenerationFile*>> getFilesSequence() const;

        IncludeGraphType& getIncludeGraph();;

        const IncludeGraphType& getIncludeGraph() const;
    };
}

#endif //UNNECESSARYENGINE_GENERATION_PLAN_H
