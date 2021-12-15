#ifndef UNNECESSARYENGINE_HEAP_H
#define UNNECESSARYENGINE_HEAP_H

#include <vector>
#include <cstddef>

namespace un {
    template<typename _ValueType>
    class MinHeap {
    private:

        class Node {
            size_t left, right;
        };

        std::vector<Node> values;


    public:
        typedef _ValueType ValueType;

        void push(ValueType value) {

        }
    };
}
#endif
