#include <unnecessary/graphics/material.h>

namespace un {

    Material::Material(un::Pipeline *shader) : shader(shader), floats(), ints() {

    }

    un::Pipeline *Material::getShader() const {
        return shader;
    }
}