#ifndef UNNECESSARYENGINE_DRAWABLE_H
#define UNNECESSARYENGINE_DRAWABLE_H

#include <vulkan/vulkan.hpp>
#include <unnecessary/graphics/material.h>
#include <unnecessary/graphics/mesh.h>
#include <unnecessary/graphics/buffers/buffer.h>

namespace un {
    struct RenderMesh {
        un::Material* material;
        un::MeshInfo* meshInfo;
    };

    struct ObjectLights {
        vk::DescriptorSet descriptorSet;
        un::ResizableBuffer buffer;
        std::vector<u32> lights;
    };
}
#endif
