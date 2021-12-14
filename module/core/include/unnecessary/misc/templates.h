//
// Created by bruno on 06/12/2021.
//

#ifndef UNNECESSARYENGINE_TEMPLATES_H
#define UNNECESSARYENGINE_TEMPLATES_H

#include <unnecessary/def.h>

namespace un {

    template<typename... Types, typename Lambda>
    void for_types(Lambda&& f) {
        (f.template operator()<Types>(), ...);
    }

    template<typename... Types, typename Lambda>
    constexpr void for_types_indexed(Lambda&& f) {
        [&f]<auto... Indices>(std::index_sequence<Indices...>) {
            (f.template operator()<Types, Indices>(), ...);
        }(std::index_sequence_for<Types...>{});
    }

    template<typename T, typename Seq>
    struct expander;

    template<typename T, std::size_t... Is>
    struct expander<T, std::index_sequence<Is...>> {
        template<typename E, std::size_t>
        using elem = E;

        using type = std::tuple<elem<T, Is>...>;
    };
    template<typename T, std::size_t Count>
    struct repeat_tuple : expander<T, std::make_index_sequence<Count>> {};

    template<typename T, typename ...Types>
    constexpr static auto index_of_type() {
        return 0;
    }

}
#endif