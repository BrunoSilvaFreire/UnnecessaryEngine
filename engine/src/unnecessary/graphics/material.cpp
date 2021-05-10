#include <unnecessary/graphics/material.h>

namespace un {

    Material::Material(un::Pipeline *shader) : shader(std::move(shader)), floats(), ints() {

    }

    un::Pipeline *Material::getShader() const {
        return shader;
    }
}