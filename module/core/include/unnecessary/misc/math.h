//
// Created by bruno on 21/12/2021.
//

#ifndef UNNECESSARYENGINE_MATH_H
#define UNNECESSARYENGINE_MATH_H
namespace un {
    template<typename T>
    static constexpr T lerp(T x, T y, float t) {
        return static_cast<T>((x + (y - x) * t));
    }

    template<typename T>
    static float inv_lerp(T x, T y, T value) {
        if (x < 0) {
            y -= x;
            value -= x;
            x = 0;
        }
        return (value - x) / (y - x);
    }

    template<typename TValue>
    static bool within_inclusive(
        TValue min,
        TValue max,
        TValue value
    ) {
        return value >= min && value <= max;
    }

    template<typename TValue>
    static bool within_exclusive(
        TValue min,
        TValue max,
        TValue value
    ) {
        return value > min && value < max;
    }
}
#endif
