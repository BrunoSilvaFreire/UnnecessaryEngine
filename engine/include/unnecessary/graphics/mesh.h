#ifndef UNNECESSARYENGINE_MESH_H
#define UNNECESSARYENGINE_MESH_H

#include <unnecessary/def.h>
#include <unnecessary/graphics/buffers/buffer.h>
#include <unnecessary/graphics/vertex_layout.h>
#include <tiny_obj_loader.h>

namespace un {
    class MeshInfo {
    private:
        u32 indexCount;
        un::BoundVertexLayout layout;
        un::Buffer vertex, index;
    public:
        MeshInfo();

        MeshInfo(
            u32 indexCount,
            const BoundVertexLayout& layout,
            const Buffer& vertex,
            const Buffer& index
        );

        u32 getIndexCount() const;

        un::Buffer& getVertex();

        un::Buffer& getIndex();
    };

    struct MeshData {
        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
    };
}
#endif
