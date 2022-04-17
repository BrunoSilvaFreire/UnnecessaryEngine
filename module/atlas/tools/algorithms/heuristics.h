#ifndef UNNECESSARYENGINE_HEURISTICS_H
#define UNNECESSARYENGINE_HEURISTICS_H

#include <unnecessary/def.h>
#include <unnecessary/math/rect.h>
#include <packer_entry.h>
#include <glm/glm.hpp>

namespace un {
    inline float bestAreaFitHeuristic(const un::packer::PackerEntry& entry, const un::uRect& rect, u32 w, u32 h) {
        return static_cast<float>(entry.getArea() - rect.getArea());
    }

    inline float bottomLeftHeuristic(const un::packer::PackerEntry& entry, const un::uRect& rect, u32 w, u32 h) {
        auto distance = glm::length(glm::vec2(rect.getOrigin()));
        return 1.0F / distance;
    }

    inline float bestShortSideFitHeuristic(const un::packer::PackerEntry& entry, const un::uRect& rect, u32 w, u32 h) {
        int shortSide = std::min(rect.getWidth(), rect.getHeight());
        int entryShortSide = std::min(entry.getWidth(), entry.getHeight());
        return static_cast<float>(std::abs(shortSide - entryShortSide));
    }
    inline float bestLongSideFitHeuristic(const un::packer::PackerEntry& entry, const un::uRect& rect, u32 w, u32 h) {
        int shortSide = std::max(rect.getWidth(), rect.getHeight());
        int entryShortSide = std::max(entry.getWidth(), entry.getHeight());
        return static_cast<float>(std::abs(shortSide - entryShortSide));
    }
}
#endif