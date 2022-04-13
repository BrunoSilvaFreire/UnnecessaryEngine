#ifndef UNNECESSARYENGINE_PACKING_ALGORITHM_H
#define UNNECESSARYENGINE_PACKING_ALGORITHM_H

#include <cstddef>
#include <filesystem>
#include <png++/png.hpp>
#include <utility>
#include <packer_entry.h>

namespace un::packer {
    struct PackingOperation {
    private:
        std::filesystem::path path;
        un::Rect<u32> destination;
    public:
        PackingOperation(
            const std::filesystem::path& path,
            const Rect<u32>& destination
        ) : path(path),
            destination(destination) { }

        const Rect<u32>& getDestination() const {
            return destination;
        }

        const std::filesystem::path& getPath() const {
            return path;
        }

    };


    struct PackingStrategy {
    private:
        u32 width, height;
        std::vector<un::packer::PackingOperation> operations;
    public:
        PackingStrategy(u32 width, u32 height, const std::vector<un::packer::PackingOperation>& operations)
            : width(width), height(height), operations(operations) { }

        u32 getWidth() const {
            return width;
        }

        u32 getHeight() const {
            return height;
        }

        const std::vector<un::packer::PackingOperation>& getOperations() const {
            return operations;
        }
    };

    class PackingAlgorithm {
    public:
        virtual un::packer::PackingStrategy operator()(std::vector<un::packer::PackerEntry> entries) = 0;
    };
}
#endif
