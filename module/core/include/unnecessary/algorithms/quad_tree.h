#ifndef UNNECESSARYENGINE_QUAD_TREE_H
#define UNNECESSARYENGINE_QUAD_TREE_H

#include <array>

namespace un {
    template<class TData>
    class QuadTree {
    private:
        TData data;
        union {
            std::array<QuadTree<TData>, 4> array;
            struct {
                QuadTree<TData> first;
                QuadTree<TData> second;
                QuadTree<TData> third;
                QuadTree<TData> fourth;
            };
        } children;
    public:
        explicit QuadTree(TData data) : data(data) {

        }

    };
}
#endif