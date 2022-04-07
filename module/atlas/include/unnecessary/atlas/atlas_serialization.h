#ifndef UNNECESSARYENGINE_ATLAS_SERIALIZATION_H
#define UNNECESSARYENGINE_ATLAS_SERIALIZATION_H

#include <unnecessary/atlas/atlas.h>
#include <unnecessary/serialization/serializer.h>

namespace un {

    class AtlasSerializer : public un::Serializer<un::Atlas> {
        void serialize(const Atlas& value, Serialized& into) override;

        Atlas deserialize(Serialized& from) override;
    };
}
#endif