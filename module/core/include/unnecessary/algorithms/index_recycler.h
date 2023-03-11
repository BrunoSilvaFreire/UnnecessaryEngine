//
// Created by bruno on 04/05/2022.
//

#ifndef UNNECESSARYENGINE_INDEX_RECYCLER_H
#define UNNECESSARYENGINE_INDEX_RECYCLER_H

#include <set>
#include <queue>

namespace un {
    template<typename TIndex = std::size_t>
    class index_recycler {
        using index_type = TIndex;

    private:
        std::queue<TIndex> _available;

    public:
        void recycle(TIndex index) {
            _available.emplace(index);
        }

        bool try_reuse(TIndex& index) {
            if (_available.empty()) {
                return false;
            }
            index = _available.front();
            _available.pop();
            return true;
        }
    };
}
#endif
