#ifndef UNNECESSARYENGINE_LAYOUT_H
#define UNNECESSARYENGINE_LAYOUT_H

#include <vector>

namespace un {
    template<typename T>
    class Layout {
    protected:
        std::vector<T> elements;
    public:
        std::vector<T>& getElements() {
            return elements;
        }

        const std::vector<T>& getElements() const {
            return elements;
        }

        const std::size_t getLength() const {
            return elements.size();
        }

        void operator+=(T&& item) {
            elements.emplace_back(std::move(item));
        }

        T& operator[](std::size_t index) {
            return elements[index];
        }

        const T& operator[](std::size_t index) const {
            return elements[index];
        }
    };
}
#endif
