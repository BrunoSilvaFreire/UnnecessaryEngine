//
// Created by bruno on 04/05/2022.
//

#ifndef UNNECESSARYENGINE_INDEX_RECYCLER_H
#define UNNECESSARYENGINE_INDEX_RECYCLER_H

#include <set>
#include <queue>

namespace un {
    template<typename TIndex = std::size_t>
    class IndexRecycler {
        using IndexType = TIndex;
    private:
        std::queue<TIndex> available;
    public:
        void recycle(TIndex index) {
            available.emplace(index);
        }

        bool try_reuse(TIndex& index) {
            if (available.empty()) {
                return false;
            }
            index = available.front();
            available.pop();
            return true;
        }


    };
}
#endif
