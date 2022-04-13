#ifndef UNNECESSARYENGINE_MAX_RECTANGLES_ALGORITHM_H
#define UNNECESSARYENGINE_MAX_RECTANGLES_ALGORITHM_H

#include <glm/glm.hpp>
#include <packing_algorithm.h>

namespace un::packer {

    class MaxRectanglesAlgorithm final : public PackingAlgorithm {
    private:
        glm::uvec2 rootMaxSize;
    public:
        explicit MaxRectanglesAlgorithm(
            glm::uvec2 rootMaxSize
        );

        PackingStrategy operator()(std::vector<un::packer::PackerEntry> entries) override;


        void removeOverlaps(Rect<u32>& rect, const Rect<u32>& other);
    };
}
#endif