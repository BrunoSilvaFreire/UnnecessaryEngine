
#ifndef UNNECESSARYENGINE_SERIALIZATION_UTILS_H
#define UNNECESSARYENGINE_SERIALIZATION_UTILS_H

#include <vector>
#include <string>
#include <unnecessary/serialization/serialized.h>

namespace un::serialization {
    template<typename TLambda>
    void deserialize_named(
        const un::Serialized& parent,
        TLambda&& forEach
    ) {
        for (const auto& [name, property] : parent.get_named_properties()) {
            forEach(name, property);
        }
    }
}
#endif