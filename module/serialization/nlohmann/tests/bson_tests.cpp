#include <unnecessary/nlohmann/bson_archiver.h>
#include <gtest/gtest.h>

TEST(nlohmann_archives, json) {
    un::serialized obj;
    obj.set("integer", 0u);
    obj.set("float", 0.0F);
    obj.set("double", 0.0);
    obj.set<std::string>("string", "string");
    un::bson_archiver archiver;
    un::byte_buffer buf = archiver.write(obj);
}
