#ifndef UNNECESSARYENGINE_PACKER_ENTRY_H
#define UNNECESSARYENGINE_PACKER_ENTRY_H

#include <unnecessary/def.h>
#include <glm/glm.hpp>
#include <filesystem>
#include "unnecessary/math/rect.h"

namespace un::packer {
    /**
     * Represent a single image to pack
     */
    class PackerEntry {
    private:
        std::filesystem::path path;
        glm::uvec2 size;
    public:
        PackerEntry() = default;
        PackerEntry(u32 width, u32 height, std::filesystem::path path);

        u32 getArea() const;

        const std::filesystem::path &getPath() const;

        u32 getWidth() const;

        u32 getHeight() const;

        const glm::uvec2 &getSize() const;

        bool fits(const un::Rect<u32> &rect) const {
            return getWidth() <= rect.getWidth() && getHeight() <= rect.getHeight();
        }

    };
}
#endif
