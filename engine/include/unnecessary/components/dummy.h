//
// Created by bruno on 03/07/2021.
//

#ifndef UNNECESSARYENGINE_DUMMY_H
#define UNNECESSARYENGINE_DUMMY_H


#define COMPONENT(...)
#define PROPERTY(...)

#include <vector>
#include <glm/glm.hpp>

namespace un {
    COMPONENT()
    struct Player {
        unsigned int id;
    };
    struct Path {
        float speed;
        float position;
        std::vector<glm::vec3> positions;
    };
    struct Orbit {
        glm::vec3 center;
        float speed;
        float radius;
        /**
         * How far along the orbit are we?
         */
        float position;
        float height;
    };
}
#endif //UNNECESSARYENGINE_DUMMY_H
