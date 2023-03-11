#ifndef UNNECESSARYENGINE_MAX_RECTANGLES_ALGORITHM_H
#define UNNECESSARYENGINE_MAX_RECTANGLES_ALGORITHM_H

#include <glm/glm.hpp>
#include <packing_algorithm.h>

namespace un::packer {
    template<float (* Heuristic)(const packer_entry& entry, const rectu& rect, u32 w, u32 h)>
    class max_rectangles_algorithm final : public packing_algorithm {
    private:
        glm::uvec2 _rootMaxSize;

        float get_score(const packer_entry& entry, rect <u32>& rect, u32 w, u32 h) const {
            return Heuristic(entry, rect, w, h);
        }

        void remove_overlaps(un::rect<u32>& rect, const un::rect<u32>& designated) const {
            un::rect<u32> cutout(
                designated.get_min_x(),
                designated.get_min_y(),
                std::numeric_limits<u32>::max() - designated.get_min_x(),
                std::numeric_limits<u32>::max() - designated.get_min_y()
            );
            const auto& toUse = designated;
            auto minX = rect.get_min_x();
            auto maxX = rect.get_max_x();
            auto minY = rect.get_min_y();
            auto maxY = rect.get_max_y();
            auto rMinX = toUse.get_min_x();
            auto rMaxX = toUse.get_max_x();
            auto rMinY = toUse.get_min_y();
            auto rMaxY = toUse.get_max_y();
            bool intersectsX =
                within_inclusive(rMinX, rMaxX, minX) || within_inclusive(rMinX, rMaxX, maxX);
            bool intersectsY =
                within_inclusive(rMinY, rMaxY, minY) || within_inclusive(rMinY, rMaxY, maxY);
            if (maxX >= rMinX && rMaxX > maxX) {
                rect.set_max_x(rMinX);
            }
            if (minX <= rMaxX && minX > rMinX) {
                rect.set_min_x(rMaxX);
            }
            if (maxY >= rMinY && rMaxY > maxY) {
                rect.set_max_y(rMinY);
            }
            if (minY <= rMaxY && minY > rMinY) {
                rect.set_min_y(rMaxY);
            }
        }

    public:
        explicit max_rectangles_algorithm(
            glm::uvec2 rootMaxSize = glm::uvec2(
                std::numeric_limits<u32>::max(),
                std::numeric_limits<u32>::max()
            )
        ) : _rootMaxSize(rootMaxSize) {
        }

        packing_strategy operator()(std::vector<packer_entry> entries) const override {
            rect<u32> root(
                glm::uvec2(0, 0),
                glm::uvec2(
                    std::numeric_limits<glm::uvec2::value_type>::max(),
                    std::numeric_limits<glm::uvec2::value_type>::max()
                )
            );
            std::vector<rect<u32>> availablePoints = {root};
            std::vector<packing_operation> operations;
            u32 w = 0, h = 0;
            std::size_t pass = 0;
            for (const auto& item : entries) {
                auto result = std::max_element(
                    availablePoints.begin(),
                    availablePoints.end(),
                    [&](rect<u32>& first, rect<u32>& second) {
                        if (!item.fits(first)) {
                            return false;
                        }
                        if (!item.fits(second)) {
                            return true;
                        }

                        float fs = get_score(item, first, w, h);
                        float ss = get_score(item, second, w, h);
                        return fs < ss;
                    }
                );
                rect<u32> rect = *result;
                auto selectedIndex = result - availablePoints.begin();
                un::rect<u32> destinated(
                    rect.get_origin(),
                    item.get_size()
                );
                availablePoints.erase(result);

                const glm::uvec2& pos = rect.get_origin();

                const auto& operation = operations.emplace_back(
                    item.get_path(),
                    destinated
                );

                glm::uvec2 sizeLeft = rect.get_size() - item.get_size();

                un::rect<u32> topPoint = rect.add_min_y(destinated.get_height());
                un::rect<u32> rightPoint = rect.add_min_x(destinated.get_width());
                availablePoints.push_back(topPoint);
                availablePoints.push_back(rightPoint);

                for (auto& toCorrect : availablePoints) {
                    remove_overlaps(toCorrect, destinated);
                }

                availablePoints.erase(
                    std::remove_if(
                        availablePoints.begin(), availablePoints.end(), [&](const auto& item) {
                            return std::any_of(
                                availablePoints.begin(), availablePoints.end(),
                                [&](const auto& other) {
                                    return item != other && other.contains(item);
                                }
                            );
                        }
                    ),
                    availablePoints.end()
                );
                w = std::max(w, destinated.get_max_x() + 1);
                h = std::max(h, destinated.get_max_y() + 1);
                pass++;
            }
            return packing_strategy(operations);
        }
    };
}
#endif
