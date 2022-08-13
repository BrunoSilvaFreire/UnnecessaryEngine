#include <cppast/visitor.hpp>
#include <unnecessary/source_analysis/cppast/transpilation.h>
#include <unnecessary/source_analysis/cppast/transpiler.h>

namespace un {


    void transpile(const cppast::cpp_file& from, un::CXXTranslationUnit& into) {
/*        un::Transpiler transpiler();
        cppast::visit(
            from,
            [](const cppast::cpp_entity& e) {
                switch (e.kind()) {
                    case cppast::cpp_entity_kind::class_t:
                    case cppast::cpp_entity_kind::class_template_t:
                    case cppast::cpp_entity_kind::enum_t:
                        return cppast::is_definition(e);
                    case cppast::cpp_entity_kind::include_directive_t: {
                        const auto& directive = dynamic_cast<const cppast::cpp_include_directive&>(e);
//                        return directive.full_path().ends_with(".h");
                        return true;
                    }
                    case cppast::cpp_entity_kind::file_t:
                    case cppast::cpp_entity_kind::namespace_t:
                        return true;
                    default:
                        return false;
                }
            },
            [this](const cppast::cpp_entity& e, cppast::visitor_info info) {
                visit(e);
            }
        );*/
    }
}
