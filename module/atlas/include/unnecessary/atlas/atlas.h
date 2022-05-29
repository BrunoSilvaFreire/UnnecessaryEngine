#ifndef UNNECESSARYENGINE_ATLAS_H
#define UNNECESSARYENGINE_ATLAS_H

#include <unnecessary/def.h>

#include <glm/glm.hpp>
#include <vector>
#include <unordered_map>
#include <uuid.h>

namespace un {
    union AtlasCoordinates {
        struct {
            u16 xMin, yMin, xMax, yMax;
        };
        struct {
            u32 min, max;
        };
    };

    struct AtlasEntry {
    private:
        AtlasCoordinates coordinates;
    public:
        const AtlasCoordinates& getCoordinates() const {
            return coordinates;
        }
    };

    class Atlas {
    private:
        std::unordered_map<uuids::uuid, u64> index;
        std::vector<AtlasEntry> entries;
    public:
        const std::unordered_map<uuids::uuid, u64>& getIndex() const {
            return index;
        }

        const std::vector<AtlasEntry>& getEntries() const {
            return entries;
        }
    };
}
#endif