#ifndef UNNECESSARYENGINE_COMMON_H
#define UNNECESSARYENGINE_COMMON_H

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <vulkan/vulkan.hpp>
#include <unnecessary/graphics/matrices.h>

namespace un {
    struct Camera {
    public:
        vk::RenderPass renderPass;
    };

    struct Parent {
    public:
        entt::entity parent;
    };

    struct Perspective {
    public:
        f32 fieldOfView, zNear, zFar, aspect;
    };

    struct Projection {
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
}
#endif
