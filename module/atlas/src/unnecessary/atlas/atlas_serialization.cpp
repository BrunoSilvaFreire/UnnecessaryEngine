#include <unnecessary/atlas/atlas_serialization.h>

namespace un {

    void AtlasSerializer::serialize(const Atlas& value, Serialized& into) {
        un::Serialized index;
        for (const auto& [key, idx] : value.getIndex()) {
            index.set(key, idx);
        }
        un::SerializedArray<un::Serialized> entries;
        for (const auto& entry : value.getEntries()) {
            const un::AtlasCoordinates& coordinates = entry.getCoordinates();
            un::Serialized& serializedEntry = entries.add();;
            serializedEntry.set("xMin", coordinates.xMin);
            serializedEntry.set("xMax", coordinates.xMax);
            serializedEntry.set("yMin", coordinates.yMin);
            serializedEntry.set("yMax", coordinates.yMax);

        }
        into.set("index", index);
        into.set("entries", std::move(entries));
    }

    Atlas AtlasSerializer::deserialize(Serialized& from) {
        return un::Atlas();
    }
};