#ifndef UNNECESSARYENGINE_MATRICES_H
#define UNNECESSARYENGINE_MATRICES_H

#include <glm/glm.hpp>

namespace un {
    struct PerObjectData {
    public:
        glm::mat4 modelMatrix;
    };
    struct Matrices {
    public:
        glm::mat4 vp;
    };
}
#endif
