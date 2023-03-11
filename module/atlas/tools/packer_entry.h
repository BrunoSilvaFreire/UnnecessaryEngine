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
    class packer_entry {
    private:
        std::filesystem::path _path;
        glm::uvec2 _size;

    public:
        packer_entry() = default;
        packer_entry(u32 width, u32 height, std::filesystem::path path);

        u32 get_area() const;

        const std::filesystem::path& get_path() const;

        u32 get_width() const;

        u32 get_height() const;

        const glm::uvec2& get_size() const;

        bool fits(const rect<u32>& rect) const {
            return get_width() <= rect.get_width() && get_height() <= rect.get_height();
        }
    };
}
#endif
