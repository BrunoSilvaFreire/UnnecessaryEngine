#include <unnecessary/graphics/material.h>
#include "../../../include/unnecessary/graphics/material.h"
#include "unnecessary/rendering/material.h"

namespace un {

    Material::Material(un::Pipeline* shader) : shader(shader), floats(), ints() {

    }

    un::Pipeline* Material::getShader() const {
        return shader;
    }

}