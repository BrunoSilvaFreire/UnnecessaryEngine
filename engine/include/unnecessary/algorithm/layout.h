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
    };
}
#endif
