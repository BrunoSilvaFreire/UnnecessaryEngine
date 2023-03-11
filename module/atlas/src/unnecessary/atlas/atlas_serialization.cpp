#include <unnecessary/atlas/atlas_serialization.h>

namespace un {
    void atlas_serializer::serialize(const atlas& value, serialized& into) {
        serialized index;
        for (const auto& [key, idx] : value.get_index()) {
            index.set(key, idx);
        }
        serialized_array<serialized> entries;
        for (const auto& entry : value.get_entries()) {
            const atlas_coordinates& coordinates = entry.get_coordinates();
            serialized& serializedEntry = entries.add();
            serializedEntry.set("xMin", coordinates.xMin);
            serializedEntry.set("xMax", coordinates.xMax);
            serializedEntry.set("yMin", coordinates.yMin);
            serializedEntry.set("yMax", coordinates.yMax);
        }
        into.set("index", index);
        into.set("entries", std::move(entries));
    }

    atlas atlas_serializer::deserialize(serialized& from) {
        return atlas();
    }
};
