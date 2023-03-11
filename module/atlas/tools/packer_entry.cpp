#include "packer_entry.h"

#include <utility>

namespace un::packer {
    packer_entry::packer_entry(
        u32 width,
        u32 height,
        std::filesystem::path path
    ) : _path(std::move(path)), _size(width, height) {
    }

    const std::filesystem::path& packer_entry::get_path() const {
        return _path;
    }

    u32 packer_entry::get_width() const {
        return _size.x;
    }

    u32 packer_entry::get_height() const {
        return _size.y;
    }

    u32 packer_entry::get_area() const {
        return _size.x * _size.y;
    }

    const glm::uvec2& packer_entry::get_size() const {
        return _size;
    }
}
