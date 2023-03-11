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
    class transpiler {
    private:
        un::ptr<cxx_translation_unit> _translationUnit;
        std::set<std::string> _alreadyParsed;
        std::shared_ptr<cppast::cpp_entity_index> _index;

    public:
        transpiler(
            un::ptr<cxx_translation_unit> unit,
            std::shared_ptr<cppast::cpp_entity_index> index
        );
        void parse(const cppast::cpp_file& file);

    private:
        void parse_class(const cppast::cpp_class& clazz);

        static cxx_type_kind to_primitive_type(const cppast::cpp_type& type);

        cxx_type to_un_type(const cppast::cpp_type& type);

        void parse_template(const cppast::cpp_class_template& clazz);

        std::string get_namespace(const cppast::cpp_entity& entt) const;

        void
        parse_field(
            cxx_composite& composite,
            const cxx_access_modifier& modifier,
            const cppast::cpp_entity& e
        );

        void parse_enum(const cppast::cpp_enum& anEnum);

        void visit(const cppast::cpp_entity& e);

        void write_ast(const cppast::cpp_file& file, const std::filesystem::path& path);

        void
        write_ast_node(
            std::ofstream& os,
            const std::string& prefix,
            const cppast::cpp_entity& e
        ) const;

        cxx_type_kind to_un_type_kind(const cppast::cpp_type& type) const;
    };
}
#endif
