#ifndef UNNECESSARYENGINE_PACKER_ENTRY_H
#define UNNECESSARYENGINE_PACKER_ENTRY_H

#include <unnecessary/def.h>
#include <filesystem>

namespace un::packer {
    class PackerEntry {
    private:
        std::filesystem::path path;
        u32 width, height;
    public:
        PackerEntry(u32 width, u32 height, std::filesystem::path path);

        u32 getArea() const;

        const std::filesystem::path& getPath() const;

        u32 getWidth() const;

        u32 getHeight() const;
    };
}
#endif
