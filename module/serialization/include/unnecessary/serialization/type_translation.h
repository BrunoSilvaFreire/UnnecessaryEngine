#ifndef UNNECESSARYENGINE_TYPE_TRANSLATION_H
#define UNNECESSARYENGINE_TYPE_TRANSLATION_H

#include <string>
#include "unnecessary/def.h"
#include "serialized_string.h"
#include <memory>

namespace un::serialization {
    template<typename TCpp>
    struct TypeTranslation {
        typedef TCpp CppSerialized;
    };


    template<>
    struct TypeTranslation<std::string> {
    public:
        typedef un::SerializedString SerializedType;
    };
    template<>
    struct TypeTranslation<u8> {
    public:
        typedef un::SerializedByte SerializedType;
    };
    template<>
    struct TypeTranslation<u16> {
    public:
        typedef un::SerializedShort SerializedType;
    };
    template<>
    struct TypeTranslation<u32> {
        typedef un::SerializedInteger SerializedType;
    };
    template<>
    struct TypeTranslation<u64> {
        typedef un::SerializedLong SerializedType;

    };
    template<>
    struct TypeTranslation<f32> {
        typedef un::SerializedFloat SerializedType;

    };
    template<>
    struct TypeTranslation<f64> {
        typedef un::SerializedDouble SerializedType;
    };

    template<typename T>
    typename TypeTranslation<T>::SerializedType create_node(const T& value) {
        using SerializedType = typename TypeTranslation<T>::SerializedType;
        return SerializedType(value);
    }

    template<typename T>
    T read_value(const typename TypeTranslation<T>::SerializedType& node) {
        using SerializedType = typename TypeTranslation<T>::SerializedType;
        return static_cast<T>(node);
    }

    template<typename T>
    T read_value(const std::shared_ptr<un::SerializedNode>& node) {
        using SerializedType = typename TypeTranslation<T>::SerializedType;
        return read_value<T>(*std::dynamic_pointer_cast<SerializedType>(node));
    }

}
#endif