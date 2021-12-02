
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
        const GPUBuffer& vertex,
        const GPUBuffer& index
    ) : indexCount(indexCount), layout(layout), vertex(vertex), index(index) {}

    GPUBuffer& MeshInfo::getVertex() {
        return vertex;
    }

    GPUBuffer& MeshInfo::getIndex() {
        return index;
    }

    const std::optional<un::GPUBuffer>& MeshInfo::getNormals() const {
        return normals;
    }

    std::optional<un::GPUBuffer>& MeshInfo::getNormals() {
        return normals;
    }

}