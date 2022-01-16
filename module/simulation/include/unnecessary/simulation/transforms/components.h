#ifndef UNNECESSARYENGINE_COMPONENTS_H
#define UNNECESSARYENGINE_COMPONENTS_H

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

namespace un {

    struct Translation {
        glm::vec3 value;
    };

    struct Rotation {
        glm::quat value;
    };

    struct Scale {
        glm::vec3 value;
    };

    struct LocalToWorld {
        glm::mat4 value;
    };
}
#endif
