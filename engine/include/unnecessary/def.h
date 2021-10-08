#ifndef UNNECESSARYENGINE_DEF_H
#define UNNECESSARYENGINE_DEF_H

#include <vulkan/vulkan.hpp>
#include <glm/glm.hpp>

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long u64;

typedef float f32;
typedef double f64;
template<typename T>
struct type_wrapper {
    using type = T;
};

template<typename... Ts, typename TF>
void for_types(TF&& f) {
    (f(type_wrapper<Ts>{}), ...);
}

#define vkCall(x) assertVkCall(x, #x);

void assertVkCall(vk::Result result, const std::string& method);

namespace un {
    typedef glm::u16vec2 Size2D;
}

#endif
