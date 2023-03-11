#include <unnecessary/nlohmann/bson_archiver.h>
#include <nlohmann/json.hpp>
#include <unnecessary/nlohmann/adapter.h>

namespace un {
    byte_buffer bson_archiver::write(const serialized& serialized) {
        nlohmann::json obj = serialization::adapt(serialized);
        auto vec = nlohmann::json::to_bson(obj);
        return byte_buffer(vec);
    }

    void bson_archiver::read(const byte_buffer& buffer, serialized& into) {
    }
}
