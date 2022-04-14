#include <algorithms/max_rectangles_algorithm.h>
#include <unnecessary/math/rect.h>
#include <unnecessary/misc/random.h>

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
            w = std::max(w, allocatedRect.getMaxX() + 1);
            h = std::max(h, allocatedRect.getMaxY() + 1);
            std::stringstream svg;
            svg << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
            svg
                << "<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 1.1//EN\" "
                   "\"http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd\">";
            u32 box = w * 3;
            u32 boxY = h * 3;
            svg << "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\" width=\"" << box << "\" height=\""
                << boxY
                << "\">";
            svg << "<rect width=\"" << box << "\" "
                << "height=\"" << boxY << "\" fill=\"black\"/>";
            for (const auto& item : availablePoints) {
                Color32 color{
                    .rgba = un::random::value<u32>()
                };
                color.a = 255;
                u32 svgYPos = item.getMinY();
                u32 svgXPos = item.getMinX();
                svg << "<rect width=\"" << item.getWidth() - 300 << "\" "
                    << "height=\"" << item.getWidth() - 300 << "\" "
                    << "x=\"" << svgXPos << "\" "
                    << "y=\"" << svgYPos << "\" "
                    << "stroke=\"#" << color
                    << std::dec << "\" "
                    << "stroke-width=\"2\""
                    << " fill=\"none\"/>";
                svg << "<text "
                    << "x=\"" << svgXPos << "\" "
                    << "y=\"" << svgYPos << "\" "
                    << "fill=\"white\">"
                    << item << "</text>";

            }
            svg << "<rect width=\"" << std::min(item.getWidth(), w) << "\" "
                << "height=\"" << std::min(item.getHeight(), h) << "\" "
                << "x=\"" << allocatedRect.getMinX() << "\" "
                << "y=\"" << allocatedRect.getMinY() << "\" "
                << "fill=\"#ff0000\" opacity=\"0.5\"/>";

            svg << "<text "
                << "x=\"" << allocatedRect.getMinX() << "\" "
                << "y=\"" << allocatedRect.getMinY() << "\" "
                << "fill=\"white\">"
                << allocatedRect << "</text>";
            svg << "</svg>";
            auto path = std::string("image_") + std::to_string(pass++) + ".svg";
            std::ofstream file(path);
            auto content = svg.str();
            file << content;
            file.close();
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

        rect.exclude(other);


    }
}
