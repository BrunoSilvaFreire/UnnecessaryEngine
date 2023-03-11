#ifndef UNNECESSARYENGINE_HEURISTICS_H
#define UNNECESSARYENGINE_HEURISTICS_H

#include <unnecessary/def.h>
#include <unnecessary/math/rect.h>
#include <packer_entry.h>
#include <glm/glm.hpp>

namespace un {
    inline float
    best_area_fit_heuristic(
        const un::packer::packer_entry& entry,
        const un::rectu& rect,
        u32 w,
        u32 h
    ) {
        return static_cast<float>(entry.get_area() - rect.get_area());
    }

    inline float
    bottom_left_heuristic(
        const un::packer::packer_entry& entry,
        const un::rectu& rect,
        u32 w,
        u32 h
    ) {
        auto distance = glm::length(glm::vec2(rect.get_origin()));
        return 1.0F / distance;
    }

    inline float
    best_short_side_fit_heuristic(
        const un::packer::packer_entry& entry,
        const un::rectu& rect,
        u32 w,
        u32 h
    ) {
        int shortSide = std::min(rect.get_width(), rect.get_height());
        int entryShortSide = std::min(entry.get_width(), entry.get_height());
        return static_cast<float>(std::abs(shortSide - entryShortSide));
    }
    inline float
    best_long_side_fit_heuristic(
        const un::packer::packer_entry& entry,
        const un::rectu& rect,
        u32 w,
        u32 h
    ) {
        int shortSide = std::max(rect.get_width(), rect.get_height());
        int entryShortSide = std::max(entry.get_width(), entry.get_height());
        return static_cast<float>(std::abs(shortSide - entryShortSide));
    }
}
#endif