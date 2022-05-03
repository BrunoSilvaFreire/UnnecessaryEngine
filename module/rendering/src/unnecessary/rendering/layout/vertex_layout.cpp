#include <unnecessary/rendering/layout/vertex_layout.h>

namespace un {
    un::VertexAttribute::VertexAttribute(
        std::string name,
        u8 count,
        u8 singleSize,
        vk::Format format
    ) : count(count), singleSize(singleSize), format(format), name(name) {

    }

    vk::Format VertexAttribute::getFormat() const {
        return format;
    }

    std::size_t VertexAttribute::getSize() const {
        return singleSize * count;
    }

    u8 VertexAttribute::getSingleSize() const {
        return singleSize;
    }

    u8 VertexAttribute::getCount() const {
        return count;
    }

    const std::string& VertexAttribute::getName() const {
        return name;
    }

    std::size_t VertexLayout::getStride() const {
        std::size_t stride = 0;
        for (const auto& element : elements) {
            stride += element.getSize();
        }
        return stride;
    }
}