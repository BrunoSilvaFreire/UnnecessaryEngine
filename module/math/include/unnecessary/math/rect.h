#ifndef UNNECESSARYENGINE_RECT_H
#define UNNECESSARYENGINE_RECT_H

#include <glm/glm.hpp>
#include <unnecessary/def.h>
#include <unnecessary/misc/math.h>
#include <unnecessary/misc/types.h>
#include <ostream>

namespace un {
    template<typename t_value>
    class rect {
    public:
        using value_type = t_value;

    private:
        glm::vec<2, value_type> _origin;
        glm::vec<2, value_type> _size;

    public:
        rect(
            value_type originX, value_type originY,
            value_type width, value_type height
        ) : _origin(originX, originY), _size(width, height) {
        }

        rect(
            const glm::vec<2, value_type>& origin,
            const glm::vec<2, value_type>& size
        ) : _origin(origin), _size(size) {
        }

        const glm::vec<2, value_type>& get_origin() const {
            return _origin;
        }

        const glm::vec<2, value_type>& get_size() const {
            return _size;
        }

        value_type get_width() const {
            return _size.x;
        }

        value_type get_height() const {
            return _size.y;
        }

        value_type get_min_x() const {
            return _origin.x;
        }

        value_type get_min_y() const {
            return _origin.y;
        }

        value_type get_max_x() const {
            if constexpr (std::is_integral<value_type>()) {
                return _origin.x + _size.x - 1;
            }
            else {
                return _origin.x + _size.x;
            }
        }

        value_type get_max_y() const {
            if constexpr (std::is_integral<value_type>()) {
                return _origin.y + _size.y - 1;
            }
            else {
                return _origin.y + _size.y;
            }
        }

        void set_min_x(value_type value) {
            _size.x -= (value - _origin.x);
            _origin.x = value;
        }

        void set_min_y(value_type value) {
            _size.y -= (value - _origin.y);
            _origin.y = value;
        }

        void set_max_x(value_type value) {
            if constexpr (std::is_integral<value_type>()) {
                _size.x = value - _origin.x + 1;
            }
            else {
                _size.x = value - _origin.x;
            }
        }

        void set_max_y(value_type value) {
            if constexpr (std::is_integral<value_type>()) {
                _size.y = value - _origin.y + 1;
            }
            else {
                _size.y = value - _origin.y;
            }
        }

        UN_AGGRESSIVE_INLINE value_type get_area() const {
            return get_width() * get_height();
        }

        void clip(const rect<value_type>& rect) {
            value_type newMaxX = std::min(get_max_x(), rect.get_max_x());
            value_type newMaxY = std::min(get_max_y(), rect.get_max_y());
            value_type newMinX = std::max(get_min_x(), rect.get_min_x());
            value_type newMinY = std::max(get_min_y(), rect.get_min_y());
            set_min_x(newMinX);
            set_min_y(newMinY);
            set_max_x(newMaxX);
            set_max_y(newMaxY);
        }

        bool contains(const rect<value_type>& other) const {
            value_type minX = get_min_x();
            value_type minY = get_min_y();
            value_type maxX = get_max_x();
            value_type maxY = get_max_y();
            value_type rMinX = other.get_min_x();
            value_type rMinY = other.get_min_y();
            value_type rMaxX = other.get_max_x();
            value_type rMaxY = other.get_max_y();
            return minX <= rMinX && maxX >= rMaxX && minY <= rMinY && maxY >= rMaxY;
        }

        bool intersects(const rect<value_type>& other) const {
            value_type minX = get_min_x();
            value_type minY = get_min_y();
            value_type maxX = get_max_x();
            value_type maxY = get_max_y();

            value_type rMinX = other.get_min_x();
            value_type rMinY = other.get_min_y();
            value_type rMaxX = other.get_max_x();
            value_type rMaxY = other.get_max_y();
            return un::within_inclusive(rMinX, rMaxX, minX) ||
                   un::within_inclusive(rMinX, rMaxX, maxX) ||
                   un::within_inclusive(rMinY, rMaxY, minY) ||
                   un::within_inclusive(rMinY, rMaxY, maxY);
        }

        rect<value_type> add_min_y(value_type value) const {
            return un::rect<value_type>(
                _origin + glm::vec<2, value_type>(0, value),
                _size - glm::vec<2, value_type>(0, value)
            );
        }

        rect<value_type> add_min_x(value_type value) const {
            return un::rect<value_type>(
                _origin + glm::vec<2, value_type>(value, 0),
                _size - glm::vec<2, value_type>(value, 0)
            );
        }

        f32 get_aspect_ratio() const {
            return std::max(get_width(), get_height()) / std::min(get_width(), get_height());
        }

        bool operator==(const rect& rhs) const {
            return _origin == rhs._origin && _size == rhs._size;
        }

        bool operator!=(const rect& rhs) const {
            return !(rhs == *this);
        }

        friend std::ostream& operator<<(std::ostream& os, const rect& rect) {
            os << "un::Rect(origin: ("
               << rect._origin.x << "," << rect._origin.y
               << "), size: ("
               << rect._size.x << "," << rect._size.y

               << "))";
            return os;
        }
    };

    using rectf = rect<f32>;
    using recti = rect<i32>;
    using rectu = rect<u32>;
}
#endif
