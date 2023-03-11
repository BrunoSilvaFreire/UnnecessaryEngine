#include <unnecessary/rendering/layout/vertex_layout.h>

namespace un {
    vertex_attribute::vertex_attribute(
        std::string name,
        u8 count,
        u8 singleSize,
        vk::Format format
    ) : _name(name), _count(count), _singleSize(singleSize), _format(format) {
    }

    vk::Format vertex_attribute::get_format() const {
        return _format;
    }

    std::size_t vertex_attribute::get_size() const {
        return _singleSize * _count;
    }

    u8 vertex_attribute::get_single_size() const {
        return _singleSize;
    }

    u8 vertex_attribute::get_count() const {
        return _count;
    }

    const std::string& vertex_attribute::get_name() const {
        return _name;
    }

    std::size_t vertex_layout::get_stride() const {
        std::size_t stride = 0;
        for (const auto& element : _elements) {
            stride += element.get_size();
        }
        return stride;
    }
}
