//
// Created by bruno on 03/05/2022.
//

#ifndef UNNECESSARYENGINE_PARSER_H
#define UNNECESSARYENGINE_PARSER_H

#include <filesystem>
#include <functional>
#include <stack>
#include <cppast/libclang_parser.hpp>
#include <utility>
#include <unnecessary/source_analysis/structures.h>
#include <unnecessary/logging.h>

namespace un::parsing {
    class ParsingOptions {
    private:
        std::string selfInclude;
        std::filesystem::path file;
        std::filesystem::path debugFile;
        std::vector<std::string> includes;
        std::shared_ptr<cppast::cpp_entity_index> indexToUse;
    public:
        ParsingOptions(std::filesystem::path file, std::vector<std::string> includes);

        const std::filesystem::path& getFile() const;

        const std::vector<std::string>& getIncludes() const;

        const std::filesystem::path& getDebugFile() const;

        void setDebugFile(const std::filesystem::path& debugFile);

        void setSelfInclude(const std::string& selfInclude);

        std::string getSelfInclude() const;

        const std::shared_ptr<cppast::cpp_entity_index>& getIndexToUse() const;

        void setIndexToUse(const std::shared_ptr<cppast::cpp_entity_index>& indexToUse);
    };

    class Parser {
    private:
        // Clang stuff
        // Parsing stuff

        un::CXXTranslationUnit translationUnit;
        un::CXXAccessModifier currentAccess = un::CXXAccessModifier::eNone;
        std::shared_ptr<un::CXXComposite> currentComposite;
        std::vector<un::CXXAttribute> macros;
        std::unique_ptr<cppast::cpp_file> result;
        std::set<std::string> alreadyParsed;
        std::shared_ptr<cppast::cpp_entity_index> index;

    public:

        Parser(const un::parsing::ParsingOptions& options);

        Parser(
            cppast::cpp_file&& file,
            std::shared_ptr<cppast::cpp_entity_index> index
        ) : result(std::move(std::make_unique<cppast::cpp_file>(std::move(file)))),
            index(std::move(index)) {

        }

        const CXXTranslationUnit& getTranslationUnit() const {
            return translationUnit;
        }

        void parse_class(const cppast::cpp_class& clazz);

        CXXTypeKind toPrimitiveType(const cppast::cpp_type& type) const;

        CXXType toUnType(const cppast::cpp_type& type);

        void parse_template(const cppast::cpp_class_template& clazz);

        std::string getNamespace(const cppast::cpp_entity& entt) const;

        void parse_field(CXXComposite& composite, const CXXAccessModifier& modifier, const cppast::cpp_entity& e);

        void parse_enum(const cppast::cpp_enum& anEnum);

        void parse_entity(const cppast::cpp_entity& file);

        void visit(const cppast::cpp_entity& e);

        void write_ast(const cppast::cpp_file& file, const std::filesystem::path& path);

        void write_ast_node(std::ofstream& os, const std::string& prefix, const cppast::cpp_entity& e) const;

        CXXTypeKind toUnTypeKind(const cppast::cpp_type& type) const;

        std::shared_ptr<cppast::cpp_entity_index> getIndex(const ParsingOptions& options);
    };
}
#endif