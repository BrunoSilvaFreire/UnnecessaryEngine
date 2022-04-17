#include "packer_entry.h"

#include <utility>

namespace un::packer {

    PackerEntry::PackerEntry(
        u32 width,
        u32 height,
        std::filesystem::path path
    ) : size(width, height), path(std::move(path)) { }

    const std::filesystem::path& PackerEntry::getPath() const {
        return path;
    }

    u32 PackerEntry::getWidth() const {
        return size.x;
    }

    u32 PackerEntry::getHeight() const {
        return size.y;
    }

    u32 PackerEntry::getArea() const {
        return size.x * size.y;
    }

    const glm::uvec2& PackerEntry::getSize() const {
        return size;
    }

}