
#include <unnecessary/graphics/drawable.h>
#include <unnecessary/graphics/mesh.h>


namespace un {
    u32 MeshInfo::getIndexCount() const {
        return indexCount;
    }

    MeshInfo::MeshInfo() : indexCount(0), layout(), vertex(), index() {

    }

    MeshInfo::MeshInfo(
        u32 indexCount,
        const BoundVertexLayout& layout,
        const Buffer& vertex,
        const Buffer& index
    ) : indexCount(indexCount), layout(layout), vertex(vertex), index(index) {}

    Buffer& MeshInfo::getVertex() {
        return vertex;
    }

    Buffer& MeshInfo::getIndex() {
        return index;
    }

}