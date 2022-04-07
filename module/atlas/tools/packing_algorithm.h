#ifndef UNNECESSARYENGINE_PACKING_ALGORITHM_H
#define UNNECESSARYENGINE_PACKING_ALGORITHM_H

#include <cstddef>
#include <filesystem>
#include <png++/png.hpp>
#include <packer_entry.h>

namespace un::packer {
    struct PackingOperation {
    private:
        std::filesystem::path path;
        std::size_t x, y;
    public:
        const std::filesystem::path& getPath() const {
            return path;
        }

        size_t getX() const {
            return x;
        }

        size_t getY() const {
            return y;
        }
    };

    struct PackingStrategy {
    private:
        std::size_t width, height;
        std::vector<un::packer::PackingOperation> operations;
    public:
        PackingStrategy(size_t width, size_t height, const std::vector<un::packer::PackingOperation>& operations)
            : width(width), height(height), operations(operations) { }

        size_t getWidth() const {
            return width;
        }

        size_t getHeight() const {
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
