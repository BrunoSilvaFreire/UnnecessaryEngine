#include <algorithms/max_rectangles_algorithm.h>
#include <unnecessary/math/rect.h>

namespace un::packer {

    PackingStrategy MaxRectanglesAlgorithm::operator()(std::vector<un::packer::PackerEntry> entries) {
        un::Rect<u32> root(
            glm::uvec2(0, 0),
            glm::uvec2(
                std::numeric_limits<glm::uvec2::value_type>::max(),
                std::numeric_limits<glm::uvec2::value_type>::max()
            )
        );
        std::vector<un::Rect<u32>> availablePoints = {root};
        std::vector<un::packer::PackingOperation> operations;
        u32 w = 0, h = 0;
        for (const auto& item : entries) {
            auto result = std::max_element(
                availablePoints.begin(),
                availablePoints.end(),
                [&](un::Rect<u32>& first, un::Rect<u32>& second) {
                    if (!item.fits(first)) {
                        return false;
                    }
                    if (!item.fits(second)) {
                        return true;
                    }
                    float fs = item.getScore(first);
                    float ss = item.getScore(second);
                    return fs < ss;
                }
            );
            un::Rect<u32> rect = *result;
            auto selectedIndex = result - availablePoints.begin();
            availablePoints.erase(result);

            const glm::uvec2& pos = rect.getOrigin();
            un::Rect<u32> allocatedRect(
                rect.getOrigin(),
                item.getSize()
            );
            const auto& operation = operations.emplace_back(
                item.getPath(),
                allocatedRect
            );

            glm::uvec2 sizeLeft = rect.getSize() - item.getSize();

            for (auto& toCorrect : availablePoints) {
                removeOverlaps(toCorrect, allocatedRect);
                toCorrect.clip(allocatedRect);
            }
            un::Rect<u32> topPoint = rect.addMinY(allocatedRect.getHeight());
            un::Rect<u32> rightPoint = rect.addMinX(allocatedRect.getWidth());
            availablePoints.push_back(topPoint);
            availablePoints.push_back(rightPoint);
            w = std::max(w, allocatedRect.getMaxX());
            h = std::max(h, allocatedRect.getMaxY());
        }
        return PackingStrategy(w, h, operations);
    }

    MaxRectanglesAlgorithm::MaxRectanglesAlgorithm(glm::uvec2 rootMaxSize) : rootMaxSize(rootMaxSize) {

    }

    void MaxRectanglesAlgorithm::removeOverlaps(Rect<u32>& rect, const Rect<u32>& other) {
        un::Rect<u32> cutout(
            other.getMinX(),
            other.getMinY(),
            std::numeric_limits<u32>::max() - other.getMinX(),
            std::numeric_limits<u32>::max() - other.getMinY()
        );

        rect.clip(cutout);


    }
}
