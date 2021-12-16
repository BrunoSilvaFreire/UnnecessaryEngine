#include <unnecessary/strings.h>

namespace un {

    std::string capitalize(std::string value) {
        value[0] = toupper(value[0]);
        return value;
    }
}