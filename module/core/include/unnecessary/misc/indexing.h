//
// Created by bruno on 07/04/2022.
//

#ifndef UNNECESSARYENGINE_INDEXING_H
#define UNNECESSARYENGINE_INDEXING_H

#include <unnecessary/def.h>

namespace un::indexing {
    template<typename TInteger = int>
    UN_AGGRESSIVE_INLINE TInteger index_of(TInteger x, TInteger y, TInteger width) {
        return (x * width) + y;
    }

    template<typename TInteger = int>
    UN_AGGRESSIVE_INLINE std::tuple<TInteger, TInteger> position_of(TInteger index, TInteger width) {
        TInteger x = index % width;
        TInteger y = index / width;
        return std::make_tuple(x, y);
    }

}
#endif
