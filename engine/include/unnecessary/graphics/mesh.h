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
        un::GPUBuffer vertex, index;
        std::optional<un::GPUBuffer> normals;
    public:
        MeshInfo();

        MeshInfo(
            u32 indexCount,
            const BoundVertexLayout& layout,
            const GPUBuffer& vertex,
            const GPUBuffer& index
        );

        u32 getIndexCount() const;

        un::GPUBuffer& getVertex();

        un::GPUBuffer& getIndex();

        std::optional<un::GPUBuffer>& getNormals();

        const std::optional<un::GPUBuffer>& getNormals() const;
    };

    template<typename IndexType>
    class IndexedMesh {
    private:
        std::vector<u8> buffer;
        std::vector<IndexType> indices;
        size_t numVertices;
    public:
        void resize(
            std::size_t newNVertices,
            const un::VertexLayout& vertexLayout
        ) {
            numVertices = newNVertices;
            buffer.resize(newNVertices * vertexLayout.getStride());
        }

        size_t getNumVertices() const {
            return numVertices;
        }

        const std::vector<u8>& getBuffer() const {
            return buffer;
        }

        const std::vector<IndexType>& getIndices() const {
            return indices;
        }

        std::vector<u8>& getBuffer() {
            return buffer;
        }

        std::vector<IndexType>& getIndices() {
            return indices;
        }

        void setIndices(const std::vector<IndexType>& indices) {
            IndexedMesh::indices = indices;
        }

        template<typename T>
        void write(
            T value,
            std::size_t index,
            std::size_t stride,
            std::size_t offset
        ) {
            T* ptr = reinterpret_cast<T*>(buffer.data() + (index * stride) + offset);
            *ptr = value;
        }
    };

    typedef IndexedMesh<u16> Mesh;

    struct ObjMeshData {
        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
    };
}
#endif
