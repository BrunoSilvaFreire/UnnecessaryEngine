#include "packer_entry.h"

namespace un::packer {

    u32 PackerEntry::getArea() const {
        return width * height;
    }

    PackerEntry::PackerEntry(u32 width, u32 height, std::filesystem::path path)
        : width(width), height(height), path(path) { }

    const std::filesystem::path& PackerEntry::getPath() const {
        return path;
    }

    u32 PackerEntry::getWidth() const {
        return width;
    }

    u32 PackerEntry::getHeight() const {
        return height;
    }
}