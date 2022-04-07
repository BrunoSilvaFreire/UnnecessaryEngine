#include <unnecessary/nlohmann/bson_archiver.h>
#include <nlohmann/json.hpp>
#include <unnecessary/nlohmann/adapter.h>


namespace un {


    un::Buffer BsonArchiver::write(const Serialized& serialized) {
        nlohmann::json obj = un::serialization::adapt(serialized);
        auto vec = nlohmann::json::to_bson(obj);
        return un::Buffer(vec);
    }

    void BsonArchiver::read(const Buffer& buffer, Serialized& into) {

    }
}