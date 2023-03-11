#include <unnecessary/nlohmann/json_archiver.h>
#include <gtest/gtest.h>

TEST(nlohmann_archives, bson) {
    un::serialized obj;
    obj.set("integer", 0u);
    obj.set("float", 0.0F);
    obj.set("double", 0.0);
    obj.set<std::string>("string", "string");
    un::json_archiver archiver;
    un::byte_buffer buf = archiver.write(obj);
    std::string json(reinterpret_cast<const char*>(buf.data()));
    GTEST_LOG_(INFO) << json;
}
