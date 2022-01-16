//
// Created by bruno on 21/12/2021.
//

#ifndef UNNECESSARYENGINE_GEOMETRY_H
#define UNNECESSARYENGINE_GEOMETRY_H

#include <unnecessary/rendering/buffers/gpu_buffer.h>
#include <cstdint>

namespace un {
    class Geometry {
    private:
        un::GPUBuffer vertexBuffer;
        un::GPUBuffer indexBuffer;
        std::size_t numVertices;
    public:
        const un::GPUBuffer& getIndexBuffer() const {
            return vertexBuffer;
        }

        const GPUBuffer& getVertexBuffer() const {
            return vertexBuffer;
        }

        std::size_t getNumVertices() const {
            return numVertices;
        }
    };
}
#endif
