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

    class local_geometry {
    private:
        un::vertex_layout _vertexLayout;
        un::byte_buffer _vertexBuffer;
        un::byte_buffer _indexBuffer;
        std::size_t _numVertices;
    public:
    };

    class device_geometry {
    private:
        un::gpu_buffer _vertexBuffer;
        un::gpu_buffer _indexBuffer;
        std::size_t _numVertices;
    public:
        const un::gpu_buffer& get_index_buffer() const {
            return _vertexBuffer;
        }

        const gpu_buffer& get_vertex_buffer() const {
            return _vertexBuffer;
        }

        std::size_t get_num_vertices() const {
            return _numVertices;
        }
    };

    class geometry {
    private:
        local_geometry _ram;
        device_geometry _vRam;
    public:
    };
}
#endif
