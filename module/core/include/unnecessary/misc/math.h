//
// Created by bruno on 21/12/2021.
//

#ifndef UNNECESSARYENGINE_MATH_H
#define UNNECESSARYENGINE_MATH_H
namespace un {
    template<typename T>
    static constexpr T lerp(T x, T y, float t) {
        return (x + (y - x) * t);
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
}
#endif
