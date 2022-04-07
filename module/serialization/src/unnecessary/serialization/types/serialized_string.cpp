#include "unnecessary/serialization/serialized_string.h"

namespace un {
    un::SerializedType SerializedString::getType() {
        return un::SerializedType::eString;
    }

    SerializedString::SerializedString(const std::string& value) : SerializedPrimitive(value) { }
}