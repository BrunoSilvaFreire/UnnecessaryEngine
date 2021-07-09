#ifndef UNNECESSARYENGINE_DRAWABLE_H
#define UNNECESSARYENGINE_DRAWABLE_H

#include <vulkan/vulkan.hpp>
#include <unnecessary/graphics/material.h>
#include <unnecessary/graphics/mesh.h>

namespace un {
    struct RenderMesh {
        un::Material *material;
        un::MeshInfo *meshInfo;
    };

}
#endif
