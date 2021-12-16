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

    template<size_t Index, typename Lambda>
    constexpr void for_indexed(Lambda&& f) {
        [&f]<auto... Indices>(std::index_sequence<Indices...>) {
            (f.template operator()<Indices>(), ...);
        }(std::make_index_sequence<Index>());
    }

    template<typename IndexSequence, typename Lambda>
    constexpr void for_indexed(Lambda&& f) {
        [&f]<auto... Indices>(std::index_sequence<Indices...>) {
            (f.template operator()<Indices>(), ...);
        }(IndexSequence());
    }

    template<typename... Types, typename Lambda>
    constexpr void for_types_indexed(Lambda&& f) {
        [&f]<auto... Indices>(std::index_sequence<Indices...>) {
            (f.template operator()<Types, Indices>(), ...);
        }(std::index_sequence_for<Types...>{});
    }

    template<typename T, size_t N>
    class repeat_tuple {
        template<typename = std::make_index_sequence<N>>
        struct impl;

        template<size_t... Is>
        struct impl<std::index_sequence<Is...>> {
            template<size_t>
            using wrap = T;

            using type = std::tuple<wrap<Is>...>;
        };

    public:
        using type = typename impl<>::type;
    };

    template<typename T, typename... Ts>
    struct index;

    template<typename T, typename... Ts>
    struct index<T, T, Ts...> : std::integral_constant<std::size_t, 0> {
    };

    template<typename T, typename U, typename... Ts>
    struct index<T, U, Ts...> : std::integral_constant<
        std::size_t,
        1 + index<T, Ts...>::value
    > {
    };


    template<typename T, typename ...Types>
    constexpr static auto index_of_type() {
        return index<T, Types...>::value;
    }

}
#endif