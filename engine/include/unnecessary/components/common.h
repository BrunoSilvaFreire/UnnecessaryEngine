#ifndef UNNECESSARYENGINE_COMMON_H
#define UNNECESSARYENGINE_COMMON_H

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <vulkan/vulkan.hpp>
namespace un {
    struct Perspective {
    public:
        glm::mat4 value;
    };

    struct Translation {
    public:
        glm::vec3 value;
    };

    struct Rotation {
    public:
        glm::quat value;
    };

    struct Scale {
    public:
        glm::vec3 value;
    };

    struct LocalToWorld {
    public:
        glm::mat4 value;
    };
    struct Drawable {
    public:
        vk::CommandBuffer buffer;
    };
}
#endif
