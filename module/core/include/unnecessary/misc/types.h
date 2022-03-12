#ifndef UNNECESSARYENGINE_TYPES_H
#define UNNECESSARYENGINE_TYPES_H

#include <string>

#ifdef __GNUG__

#include <cstdlib>
#include <memory>
#include <cxxabi.h>

#else
#include <typeinfo>
#endif
namespace un {
#ifdef __GNUG__

    std::string demangle(const char* value);

    template<typename T>
    std::string type_name_of() {
        return demangle(typeid(T).name());
    }

    std::string type_name_of(const std::type_info& info);

#else
    template<typename T>
    std::string type_name_of() {
        return typeid(T).name();
    }

    std::string demangle(const char* value);
#endif
}
#endif