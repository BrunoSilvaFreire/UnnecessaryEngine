#ifndef UNNECESSARYENGINE_TYPE_TRANSLATION_H
#define UNNECESSARYENGINE_TYPE_TRANSLATION_H

#include <string>
#include "unnecessary/def.h"
#include "serialized_string.h"
#include <memory>

namespace un::serialization {
    template<typename TCpp>
    struct type_translation {
        using cpp_serialized = TCpp;
    };

    template<>
    struct type_translation<std::string> {
    public:
        typedef un::serialized_string serialized_type;
    };
    template<>
    struct type_translation<u8> {
    public:
        typedef un::serialized_byte serialized_type;
    };
    template<>
    struct type_translation<u16> {
    public:
        typedef un::serialized_short serialized_type;
    };
    template<>
    struct type_translation<u32> {
        typedef un::serialized_integer serialized_type;
    };
    template<>
    struct type_translation<u64> {
        typedef un::serialized_long serialized_type;

    };
    template<>
    struct type_translation<f32> {
        typedef un::serialized_float serialized_type;

    };
    template<>
    struct type_translation<f64> {
        typedef un::serialized_double serialized_type;
    };

    template<typename T>
    typename type_translation<T>::serialized_type create_node(const T& value) {
        using serialized_type = typename type_translation<T>::serialized_type;
        return serialized_type(value);
    }

    template<typename T>
    T read_value(const typename type_translation<T>::serialized_type& node) {
        using serialized_type = typename type_translation<T>::serialized_type;
        return static_cast<T>(node);
    }

    template<typename T>
    T read_value(const std::shared_ptr<un::serialized_node>& node) {
        using serialized_type = typename type_translation<T>::serialized_type;
        return read_value<T>(*std::dynamic_pointer_cast<serialized_type>(node));
    }

}
#endif