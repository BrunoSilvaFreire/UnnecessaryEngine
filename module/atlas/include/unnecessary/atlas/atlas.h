#ifndef UNNECESSARYENGINE_ATLAS_H
#define UNNECESSARYENGINE_ATLAS_H

#include <unnecessary/def.h>

#include <glm/glm.hpp>
#include <vector>
#include <unordered_map>
#include <uuid.h>

namespace un {
    union atlas_coordinates {
        struct {
            u16 xMin, yMin, xMax, yMax;
        };

        struct {
            u32 min, max;
        };
    };

    struct atlas_entry {
    private:
        atlas_coordinates _coordinates;

    public:
        const atlas_coordinates& get_coordinates() const {
            return _coordinates;
        }
    };

    class atlas {
    private:
        std::unordered_map<uuids::uuid, u64> _index;
        std::vector<atlas_entry> _entries;

    public:
        const std::unordered_map<uuids::uuid, u64>& get_index() const {
            return _index;
        }

        const std::vector<atlas_entry>& get_entries() const {
            return _entries;
        }
    };
}
#endif
