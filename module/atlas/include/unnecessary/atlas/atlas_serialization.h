#ifndef UNNECESSARYENGINE_ATLAS_SERIALIZATION_H
#define UNNECESSARYENGINE_ATLAS_SERIALIZATION_H

#include <unnecessary/atlas/atlas.h>
#include <unnecessary/serialization/serializer.h>

namespace un {
    class atlas_serializer : public serializer<atlas> {
        void serialize(const atlas& value, serialized& into) override;

        atlas deserialize(serialized& from) override;
    };
}
#endif
