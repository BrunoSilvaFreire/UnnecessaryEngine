#include <unnecessary/nlohmann/bson_archiver.h>
#include <gtest/gtest.h>


TEST(nlohmann_archives, json) {
    un::Serialized obj;
    obj.set("integer", 0u);
    obj.set("float", 0.0F);
    obj.set("double", 0.0);
    obj.set<std::string>("string", "string");
    un::BsonArchiver archiver;
    un::Buffer buf = archiver.write(obj);

}
