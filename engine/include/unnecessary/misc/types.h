#ifndef UNNECESSARYENGINE_TYPES_H
#define UNNECESSARYENGINE_TYPES_H
#ifdef __GNUG__

#include <cstdlib>
#include <memory>
#include <cxxabi.h>

#endif
namespace un {
#ifdef __GNUG__

    template<typename T>
    std::string typeNameOf() {
        //https://stackoverflow.com/questions/281818/unmangling-the-result-of-stdtype-infoname
        int status = -4;

        // enable c++11 by passing the flag -std=c++11 to g++
        std::unique_ptr<char, void (*)(void*)> res{
            abi::__cxa_demangle(typeid(T).name(), NULL, NULL, &status),
            std::free
        };

        return (status == 0) ? res.get() : typeid(T).name();
    }

    std::string demangle(const char* value);

#else
    template<typename T>
    std::string typeNameOf() {
        return typeid(T).name();
    }

    std::string demangle(const char* value) {
        return value;
    }
#endif
}
#endif