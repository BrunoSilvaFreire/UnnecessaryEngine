#ifndef UNNECESSARYENGINE_PACKING_ALGORITHM_H
#define UNNECESSARYENGINE_PACKING_ALGORITHM_H

#include <cstddef>
#include <filesystem>
#include <png++/png.hpp>
#include <utility>
#include <packer_entry.h>

namespace un::packer {
    struct packing_operation {
    private:
        std::filesystem::path _path;
        rect<u32> _destination;

    public:
        packing_operation(
            const std::filesystem::path& path,
            const rect<u32>& destination
        ) : _path(path),
            _destination(destination) {
        }

        const rect<u32>& get_destination() const {
            return _destination;
        }

        const std::filesystem::path& get_path() const {
            return _path;
        }
    };

    struct packing_strategy {
    private:
        u32 _width, _height;
        std::vector<packing_operation> _operations;

    public:
        packing_strategy(const std::vector<packing_operation>& operations)
            : _width(0), _height(0), _operations(operations) {
            for (const auto& operation : operations) {
                const auto& destination = operation.get_destination();
                _width = std::max(_width, destination.get_max_x() + 1);
                _height = std::max(_height, destination.get_max_y() + 1);
            }
        }

        u32 get_width() const {
            return _width;
        }

        u32 get_height() const {
            return _height;
        }

        const std::vector<packing_operation>& get_operations() const {
            return _operations;
        }
    };

    class packing_algorithm {
    public:
        virtual packing_strategy operator()(std::vector<packer_entry> entries) const = 0;
    };
}
#endif
