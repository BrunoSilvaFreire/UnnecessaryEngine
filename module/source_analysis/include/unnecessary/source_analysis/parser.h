//
// Created by bruno on 03/05/2022.
//

#ifndef UNNECESSARYENGINE_PARSER_H
#define UNNECESSARYENGINE_PARSER_H

#include <filesystem>
#include <functional>
#include <stack>
#include <cppast/libclang_parser.hpp>
#include <unnecessary/source_analysis/structures.h>
#include <unnecessary/logging.h>

namespace un::parsing {
    class ParsingOptions {
    private:
        std::filesystem::path file;
        std::vector<std::string> includes;

    public:
        ParsingOptions(std::filesystem::path file, std::vector<std::string> includes);

        const std::filesystem::path& getFile() const;

        const std::vector<std::string>& getIncludes() const;
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

    public:

        Parser(const un::parsing::ParsingOptions& options);

        const CXXTranslationUnit& getTranslationUnit() const {
            return translationUnit;
        }

        void parse_class(const cppast::cpp_class& clazz);

        CXXTypeKind toPrimitiveType(const cppast::cpp_type& type) const;

        CXXType toUnType(const cppast::cpp_type& type);

        void parse_template(const cppast::cpp_class_template& clazz);

        std::string getNamespace(const cppast::cpp_entity& entt) const;

        void parse_field(CXXComposite& composite, const CXXAccessModifier& modifier, const cppast::cpp_entity& e);
    };
}
#endif