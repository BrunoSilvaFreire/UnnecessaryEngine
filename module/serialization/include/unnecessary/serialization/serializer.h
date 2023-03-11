#ifndef UNNECESSARYENGINE_SERIALIZER_H
#define UNNECESSARYENGINE_SERIALIZER_H

#include <unnecessary/serialization/serialized.h>

namespace un {
    namespace serialization {
        template<typename TValue>
        void serialize_structure(const TValue& value, un::serialized& into);

        template<typename TValue>
        TValue deserialize_structure(un::serialized& from);

        template<typename TValue>
        void serialize_inline(const std::string& key, const TValue& value, un::serialized& into);

        template<typename TValue>
        TValue deserialize_inline(const std::string& key, un::serialized& from);
    }

    template<typename TValue>
    class serializer {
        virtual void serialize(const TValue& value, un::serialized& into) = 0;

        virtual TValue deserialize(un::serialized& from) = 0;
    };
}
#endif
