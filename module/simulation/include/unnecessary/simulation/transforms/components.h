#ifndef UNNECESSARYENGINE_COMPONENTS_H
#define UNNECESSARYENGINE_COMPONENTS_H

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

namespace un {
    struct translation {
        glm::vec3 value;
    };

    struct rotation {
        glm::quat value;
    };

    struct scale {
        glm::vec3 value;
    };

    struct local_to_world {
        glm::mat4 value;
    };
}
#endif
