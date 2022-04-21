#ifndef UNNECESSARYENGINE_MAX_RECTANGLES_ALGORITHM_H
#define UNNECESSARYENGINE_MAX_RECTANGLES_ALGORITHM_H

#include <glm/glm.hpp>
#include <packing_algorithm.h>

namespace un::packer {
    template<float (* Heuristic)(const un::packer::PackerEntry& entry, const un::uRect& rect, u32 w, u32 h)>
    class MaxRectanglesAlgorithm final : public PackingAlgorithm {
    private:
        glm::uvec2 rootMaxSize;

        inline float getScore(const PackerEntry& entry, Rect<u32>& rect, u32 w, u32 h) const {
            return Heuristic(entry, rect, w, h);
        }


        void removeOverlaps(Rect<u32>& rect, const Rect<u32>& designated) const {
            un::Rect<u32> cutout(
                designated.getMinX(),
                designated.getMinY(),
                std::numeric_limits<u32>::max() - designated.getMinX(),
                std::numeric_limits<u32>::max() - designated.getMinY()
            );
            const auto& toUse = designated;
            auto minX = rect.getMinX();
            auto maxX = rect.getMaxX();
            auto minY = rect.getMinY();
            auto maxY = rect.getMaxY();
            auto rMinX = toUse.getMinX();
            auto rMaxX = toUse.getMaxX();
            auto rMinY = toUse.getMinY();
            auto rMaxY = toUse.getMaxY();
            bool intersectsX = un::within_inclusive(rMinX, rMaxX, minX) || un::within_inclusive(rMinX, rMaxX, maxX);
            bool intersectsY = un::within_inclusive(rMinY, rMaxY, minY) || un::within_inclusive(rMinY, rMaxY, maxY);
            if (maxX >= rMinX && rMaxX > maxX) {
                rect.setMaxX(rMinX);
            }
            if (minX <= rMaxX && minX > rMinX) {
                rect.setMinX(rMaxX);
            }
            if (maxY >= rMinY && rMaxY > maxY) {
                rect.setMaxY(rMinY);
            }
            if (minY <= rMaxY && minY > rMinY) {
                rect.setMinY(rMaxY);
            }
        }

    public:
        explicit MaxRectanglesAlgorithm(
            glm::uvec2 rootMaxSize = glm::uvec2(
                std::numeric_limits<u32>::max(),
                std::numeric_limits<u32>::max()
            )
        ) : rootMaxSize(rootMaxSize) {

        }


        PackingStrategy operator()(std::vector<un::packer::PackerEntry> entries) const override {
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
            std::size_t pass = 0;
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

                        float fs = getScore(item, first, w, h);
                        float ss = getScore(item, second, w, h);
                        return fs < ss;
                    }
                );
                un::Rect<u32> rect = *result;
                auto selectedIndex = result - availablePoints.begin();
                un::Rect<u32> destinated(
                    rect.getOrigin(),
                    item.getSize()
                );
                availablePoints.erase(result);

                const glm::uvec2& pos = rect.getOrigin();

                const auto& operation = operations.emplace_back(
                    item.getPath(),
                    destinated
                );

                glm::uvec2 sizeLeft = rect.getSize() - item.getSize();

                un::Rect<u32> topPoint = rect.addMinY(destinated.getHeight());
                un::Rect<u32> rightPoint = rect.addMinX(destinated.getWidth());
                availablePoints.push_back(topPoint);
                availablePoints.push_back(rightPoint);
                for (auto& toCorrect : availablePoints) {
                    removeOverlaps(toCorrect, destinated);
                }
                availablePoints.erase(
                    std::remove_if(availablePoints.begin(), availablePoints.end(), [&](const auto& item) {
                        return std::any_of(
                            availablePoints.begin(), availablePoints.end(),
                            [&](const auto& other) {
                                return item != other && other.contains(item);
                            }
                        );
                    }),
                    availablePoints.end()
                );
                w = std::max(w, destinated.getMaxX() + 1);
                h = std::max(h, destinated.getMaxY() + 1);
                pass++;
            }
            return PackingStrategy(operations);
        }

    };
}
#endif