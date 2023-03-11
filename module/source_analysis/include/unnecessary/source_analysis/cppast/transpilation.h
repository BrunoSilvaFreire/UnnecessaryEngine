#ifndef UNNECESSARYENGINE_TRANSPILATION_H
#define UNNECESSARYENGINE_TRANSPILATION_H

#include <cppast/cppast_fwd.hpp>
#include <cppast/cpp_class.hpp>
#include <cppast/cpp_file.hpp>
#include <cppast/cpp_member_variable.hpp>
#include <cppast/cpp_enum.hpp>
#include <cppast/cpp_preprocessor.hpp>
#include <unnecessary/source_analysis/structures.h>

namespace un {
    void transpile(const cppast::cpp_file& from, cxx_translation_unit& into);
}
#endif
