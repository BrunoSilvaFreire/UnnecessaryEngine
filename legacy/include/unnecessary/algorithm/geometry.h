
#ifndef UNNECESSARYENGINE_GEOMETRY_H
#define UNNECESSARYENGINE_GEOMETRY_H

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

namespace un {
    namespace transformations {
        inline glm::vec3 up(glm::vec3 forward) {
            return forward * glm::quat(glm::vec3(0, 90, 0));
        }
    }
}
#endif
