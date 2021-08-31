#ifndef UNNECESSARYENGINE_COMMON_H
#define UNNECESSARYENGINE_COMMON_H

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <vulkan/vulkan.hpp>
#include <entt/entt.hpp>
#include <unnecessary/def.h>
#include <unnecessary/graphics/matrices.h>
#include <unnecessary/graphics/buffers/buffer.h>

#define COMPONENT(...)
#define PROPERTY(...)
namespace un {
    COMPONENT(REQUIRES = [Projection])
    struct Camera {
    public:
        vk::DescriptorSet cameraDescriptorSet;
        un::Buffer cameraDescriptorBuffer;
    };

    COMPONENT()
    struct Name {
    public:
        PROPERTY()
        std::string value;
    };

    COMPONENT()
    struct Parent {
    public:
        entt::entity parent;
    };

    COMPONENT()
    struct Perspective {
    public:
        PROPERTY()
        f32 fieldOfView;
        PROPERTY()
        f32 zNear;
        PROPERTY()
        f32 zFar;
        PROPERTY()
        f32 aspect;
    };

    COMPONENT()
    struct Projection {
    public:
        glm::mat4 value;
    };

    COMPONENT()
    struct Translation {
    public:
        PROPERTY()
        glm::vec3 value;
    };

    COMPONENT()
    struct Velocity {
    public:
        glm::vec3 value;
    };

    COMPONENT()
    struct Rotation {
    public:
        glm::quat value;
    };

    COMPONENT()
    struct Scale {
    public:
        glm::vec3 value;
    };

    COMPONENT()
    struct LocalToWorld {
    public:
        glm::mat4 value;
    };
}
#endif
