#ifndef UNNECESSARYENGINE_LAYOUT_H
#define UNNECESSARYENGINE_LAYOUT_H

#include <vector>

namespace un {
    template<typename T>
    class Layout {
    private:
        std::vector<T> elements;
    public:
        std::vector<T> &getElements() {
            return elements;
        }
    };
}
#endif
