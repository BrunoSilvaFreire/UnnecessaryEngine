#ifndef UNNECESSARYENGINE_RECT_H
#define UNNECESSARYENGINE_RECT_H

#include <glm/glm.hpp>
#include <unnecessary/def.h>
#include <unnecessary/misc/math.h>
#include <unnecessary/misc/types.h>
#include <ostream>

namespace un {
    template<typename TValue>
    class Rect {
    private:
        glm::vec<2, TValue> origin;
        glm::vec<2, TValue> size;
    public:
        Rect(
            TValue originX, TValue originY,
            TValue width, TValue height
        ) : origin(originX, originY), size(width, height) {

        }

        Rect(
            const glm::vec<2, TValue>& origin,
            const glm::vec<2, TValue>& size
        ) : origin(origin), size(size) { }

        const glm::vec<2, TValue>& getOrigin() const {
            return origin;
        }

        const glm::vec<2, TValue>& getSize() const {
            return size;
        }

        TValue getWidth() const {
            return size.x;
        }

        TValue getHeight() const {
            return size.y;
        }

        TValue getMinX() const {
            return origin.x;
        }

        TValue getMinY() const {
            return origin.y;
        }

        TValue getMaxX() const {
            if constexpr(std::is_integral<TValue>()) {
                return origin.x + size.x - 1;
            } else {
                return origin.x + size.x;
            }
        }

        TValue getMaxY() const {
            if constexpr(std::is_integral<TValue>()) {
                return origin.y + size.y - 1;
            } else {
                return origin.y + size.y;
            }
        }

        void setMinX(TValue value) {
            size.x -= (value - origin.x);
            origin.x = value;
        }

        void setMinY(TValue value) {
            size.y -= (value - origin.y);
            origin.y = value;
        }

        void setMaxX(TValue value) {
            if constexpr(std::is_integral<TValue>()) {
                size.x = value - origin.x + 1;
            } else {
                size.x = value - origin.x;
            }
        }

        void setMaxY(TValue value) {
            if constexpr(std::is_integral<TValue>()) {
                size.y = value - origin.y + 1;
            } else {
                size.y = value - origin.y;
            }
        }

        UN_AGGRESSIVE_INLINE TValue getArea() const {
            return getWidth() * getHeight();
        }

        void clip(const Rect<TValue>& rect) {
            TValue newMaxX = std::min(getMaxX(), rect.getMaxX());
            TValue newMaxY = std::min(getMaxY(), rect.getMaxY());
            TValue newMinX = std::max(getMinX(), rect.getMinX());
            TValue newMinY = std::max(getMinY(), rect.getMinY());
            setMinX(newMinX);
            setMinY(newMinY);
            setMaxX(newMaxX);
            setMaxY(newMaxY);
        }

        bool contains(const un::Rect<TValue>& other) const {
            TValue minX = getMinX();
            TValue minY = getMinY();
            TValue maxX = getMaxX();
            TValue maxY = getMaxY();
            TValue rMinX = other.getMinX();
            TValue rMinY = other.getMinY();
            TValue rMaxX = other.getMaxX();
            TValue rMaxY = other.getMaxY();
            return minX <= rMinX && maxX >= rMaxX && minY <= rMinY && maxY >= rMaxY;
        }

        bool intersects(const un::Rect<TValue>& other) const {
            TValue minX = getMinX();
            TValue minY = getMinY();
            TValue maxX = getMaxX();
            TValue maxY = getMaxY();

            TValue rMinX = other.getMinX();
            TValue rMinY = other.getMinY();
            TValue rMaxX = other.getMaxX();
            TValue rMaxY = other.getMaxY();
            return un::within_inclusive(rMinX, rMaxX, minX) ||
                   un::within_inclusive(rMinX, rMaxX, maxX) ||
                   un::within_inclusive(rMinY, rMaxY, minY) ||
                   un::within_inclusive(rMinY, rMaxY, maxY);
        }

        un::Rect<TValue> addMinY(TValue value) const {
            return un::Rect<TValue>(
                origin + glm::vec<2, TValue>(0, value),
                size - glm::vec<2, TValue>(0, value)
            );
        }

        un::Rect<TValue> addMinX(TValue value) const {
            return un::Rect<TValue>(
                origin + glm::vec<2, TValue>(value, 0),
                size - glm::vec<2, TValue>(value, 0)
            );
        }

        f32 getAspectRatio() const {
            return std::max(getWidth(), getHeight()) / std::min(getWidth(), getHeight());
        }

        bool operator==(const Rect& rhs) const {
            return origin == rhs.origin && size == rhs.size;
        }

        bool operator!=(const Rect& rhs) const {
            return !(rhs == *this);
        }

        friend std::ostream& operator<<(std::ostream& os, const Rect& rect) {
            os << "un::Rect(origin: ("
               << rect.origin.x << "," << rect.origin.y
               << "), size: ("
               << rect.size.x << "," << rect.size.y

               << "))";
            return os;
        }
    };

    typedef un::Rect<f32> fRect;
    typedef un::Rect<i32> iRect;
    typedef un::Rect<u32> uRect;
}
#endif
