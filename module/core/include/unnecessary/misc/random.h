#ifndef UNNECESSARYENGINE_RANDOM_H
#define UNNECESSARYENGINE_RANDOM_H

#include <cstdio>
#include <random>
#include <unnecessary/misc/math.h>

namespace un {
    namespace random {
        static std::random_device device;
        static std::mt19937 engine(device());

        float value(float min = 0, float max = 1) {
            return std::uniform_real_distribution<>(min, max)(engine);
        }
    }
}
#endif
