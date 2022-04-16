#include <algorithms/max_rectangles_algorithm.h>
#include <unnecessary/math/rect.h>
#include <unnecessary/misc/random.h>
#include <unnecessary/logging.h>
#include <sstream>

namespace un::packer {
    union Color32 {
        struct {
            u8 a, r, g, b;
        };
        u32 rgba;

        friend std::ostream& operator<<(std::ostream& stream, const Color32& col) {

            stream << std::setfill('0') << std::setw(sizeof(uint8_t) * 2) << std::hex << (int) col.r;
            stream << std::setfill('0') << std::setw(sizeof(uint8_t) * 2) << std::hex << (int) col.g;
            stream << std::setfill('0') << std::setw(sizeof(uint8_t) * 2) << std::hex << (int) col.b;
            return stream;
        }
    };

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
                    float fs = item.getScore(first);
                    float ss = item.getScore(second);
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
        return PackingStrategy(w, h, operations);
    }

    MaxRectanglesAlgorithm::MaxRectanglesAlgorithm(glm::uvec2 rootMaxSize) : rootMaxSize(rootMaxSize) {

    }

    void MaxRectanglesAlgorithm::removeOverlaps(Rect<u32>& rect, const Rect<u32>& designated) {
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
}
