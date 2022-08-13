//
// Created by bruno on 03/05/2022.
//

#ifndef UNNECESSARYENGINE_TRANSPILER_H
#define UNNECESSARYENGINE_TRANSPILER_H

#include <filesystem>
#include <functional>
#include <stack>
#include <cppast/libclang_parser.hpp>
#include <utility>
#include <unnecessary/source_analysis/structures.h>
#include <unnecessary/logging.h>

namespace un {

    class Transpiler {
    private:
        un::ptr<un::CXXTranslationUnit> translationUnit;
        std::set<std::string> alreadyParsed;
        std::shared_ptr<cppast::cpp_entity_index> index;
    public:

        Transpiler(
            un::ptr<un::CXXTranslationUnit> unit,
            std::shared_ptr<cppast::cpp_entity_index> index
        );
        void parse(const cppast::cpp_file& file);

    private:
        void parse_class(const cppast::cpp_class& clazz);

        CXXTypeKind toPrimitiveType(const cppast::cpp_type& type) const;

        CXXType toUnType(const cppast::cpp_type& type);

        void parse_template(const cppast::cpp_class_template& clazz);

        std::string getNamespace(const cppast::cpp_entity& entt) const;

        void parse_field(CXXComposite& composite, const CXXAccessModifier& modifier, const cppast::cpp_entity& e);

        void parse_enum(const cppast::cpp_enum& anEnum);

        void visit(const cppast::cpp_entity& e);

        void write_ast(const cppast::cpp_file& file, const std::filesystem::path& path);

        void write_ast_node(std::ofstream& os, const std::string& prefix, const cppast::cpp_entity& e) const;

        CXXTypeKind toUnTypeKind(const cppast::cpp_type& type) const;

    };
}
#endif