//
// Created by bruno on 06/12/2021.
//

#ifndef UNNECESSARYENGINE_TEMPLATES_H
#define UNNECESSARYENGINE_TEMPLATES_H

#include <unnecessary/def.h>

template<typename T>
struct type_wrapper {
    using type = T;
};

template<typename... Types, typename Lambda>
void for_types(Lambda&& f) {
    (f(type_wrapper<Types>{}), ...);
}

template<typename... Types, typename Lambda>
constexpr void for_types_indexed(Lambda&& f) {
    [&f]<auto... Is>(std::index_sequence<Is...>) {
        (f.template operator()<Types, Is>(), ...);
    }(std::index_sequence_for<Types...>{});
}

namespace un {

    template<typename T, typename ...Types>
    constexpr static auto index_of_type() {
        return 0;
    }

}
#endif