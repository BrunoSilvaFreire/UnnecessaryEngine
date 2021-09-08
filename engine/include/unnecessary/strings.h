#ifndef UNNECESSARYENGINE_STRINGS_H
#define UNNECESSARYENGINE_STRINGS_H

#include <string>

namespace un {
    template<typename T>
    std::string to_string(const T& value) {
        return std::to_string(value);
    }
}
#endif
