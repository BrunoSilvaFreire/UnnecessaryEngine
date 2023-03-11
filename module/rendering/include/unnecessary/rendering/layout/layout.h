#ifndef UNNECESSARYENGINE_LAYOUT_H
#define UNNECESSARYENGINE_LAYOUT_H

#include <vector>

namespace un {
    template<typename T>
    class layout {
    protected:
        std::vector<T> _elements;

    public:
        std::vector<T>& get_elements() {
            return _elements;
        }

        const std::vector<T>& get_elements() const {
            return _elements;
        }

        const std::size_t get_length() const {
            return _elements.size();
        }

        void operator+=(T&& item) {
            _elements.emplace_back(std::move(item));
        }

        T& operator[](std::size_t index) {
            return _elements[index];
        }

        const T& operator[](std::size_t index) const {
            return _elements[index];
        }
    };
}
#endif
