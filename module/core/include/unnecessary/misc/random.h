#ifndef UNNECESSARYENGINE_RANDOM_H
#define UNNECESSARYENGINE_RANDOM_H

#include <cstdio>
#include <random>
#include <unnecessary/misc/math.h>

namespace un {
    namespace random {
        static std::random_device device;
        static std::mt19937 engine(device());

        template<typename TValue>
        constexpr bool can_be_randomized() {
            return std::is_integral<TValue>() || std::is_scalar<TValue>();
        }

        template<typename TValue>
        TValue value(TValue min, TValue max) {
            static_assert(can_be_randomized<TValue>());
            if constexpr (std::is_integral<TValue>()) {
                return std::uniform_int_distribution<TValue>(min, max)(engine);
            }
            if constexpr (std::is_floating_point<TValue>()) {
                return std::uniform_real_distribution<TValue>(min, max)(engine);
            }
        }

        template<typename TValue>
        TValue value() {
            return value<TValue>(
                std::numeric_limits<TValue>::min(),
                std::numeric_limits<TValue>::max());
        }

    }
}
#endif
