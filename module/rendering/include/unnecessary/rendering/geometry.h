//
// Created by bruno on 21/12/2021.
//

#ifndef UNNECESSARYENGINE_GEOMETRY_H
#define UNNECESSARYENGINE_GEOMETRY_H

#include "unnecessary/memory/membuf.h"
#include <unnecessary/rendering/buffers/gpu_buffer.h>
#include <unnecessary/rendering/layout/vertex_layout.h>
#include <cstdint>

namespace un {

    class LocalGeometry {
    private:
        un::VertexLayout vertexLayout;
        un::Buffer vertexBuffer;
        un::Buffer indexBuffer;
        std::size_t numVertices;
    public:
    };

    class DeviceGeometry {
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

    class Geometry {
    private:
        LocalGeometry ram;
        DeviceGeometry vRam;
    public:
    };
}
#endif
