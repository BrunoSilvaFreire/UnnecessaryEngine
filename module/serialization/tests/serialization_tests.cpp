#include "unnecessary/serialization/serialized.h"
#include "unnecessary/serialization/serialized_string.h"
#include <gtest/gtest.h>

template<typename T>
void access(const un::serialized& obj, const std::string& key, T expected) {
    T result{};
    bool found = obj.try_get<T>(key, result);
    ASSERT_TRUE(found) << "Unable to find object named " << key << ".";
    if (!found) {
        return;
    }
    ASSERT_TRUE(result == expected);
}

TEST(serialization, serialized_access) {
    un::serialized obj;
    obj.set("integer", 0u);
    obj.set("float", 0.0F);
    obj.set("double", 0.0);
    obj.set<std::string>("string", "string");
    access(obj, "integer", 0u);
    access(obj, "float", 0.0F);
    access(obj, "double", 0.0);
    access<std::string>(obj, "string", "string");
}
