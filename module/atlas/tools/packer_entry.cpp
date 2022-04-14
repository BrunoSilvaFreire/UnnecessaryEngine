#include "packer_entry.h"

namespace un::packer {

    u32 PackerEntry::getArea() const {
        return size.x * size.y;
    }

    PackerEntry::PackerEntry(u32 width, u32 height, std::filesystem::path path)
        : size(width, height), path(path) { }

    const std::filesystem::path& PackerEntry::getPath() const {
        return path;
    }

    u32 PackerEntry::getWidth() const {
        return size.x;
    }

    u32 PackerEntry::getHeight() const {
        return size.y;
    }

    const glm::uvec2& PackerEntry::getSize() const {
        return size;
    }

    float PackerEntry::getScore(Rect<u32>& rect) const {
        auto area = rect.getArea();
        auto aspectRatio = rect.getAspectRatio();
        return (1.0F / static_cast<float>(area));
    }
}