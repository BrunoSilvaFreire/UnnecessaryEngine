#include "unnecessary/serialization/serialized_string.h"

namespace un {
    serialized_type serialized_string::get_type() {
        return string;
    }

    serialized_string::serialized_string(const std::string& value) : serialized_primitive(value) {
    }
}
