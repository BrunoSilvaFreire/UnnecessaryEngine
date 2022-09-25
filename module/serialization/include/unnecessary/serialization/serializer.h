#ifndef UNNECESSARYENGINE_SERIALIZER_H
#define UNNECESSARYENGINE_SERIALIZER_H

#include <unnecessary/serialization/serialized.h>

namespace un {
    namespace serialization {
        template<typename TValue>
        void serialize_structure(const TValue& value, un::Serialized& into);

        template<typename TValue>
        TValue deserialize_structure(un::Serialized& from);

        template<typename TValue>
        void serialize_inline(const std::string& key, const TValue& value, un::Serialized& into);

        template<typename TValue>
        TValue deserialize_inline(const std::string& key, un::Serialized& from);
    }

    template<typename TValue>
    class Serializer {
        virtual void serialize(const TValue& value, un::Serialized& into) = 0;

        virtual TValue deserialize(un::Serialized& from) = 0;
    };
}
#endif
