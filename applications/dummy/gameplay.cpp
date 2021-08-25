#include "gameplay.h"
#include <unnecessary/components/dummy.h>
#include <unnecessary/components/common.h>
#include <unnecessary/systems/transform.h>

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

    static bool approximately(float a, float b);

    template<typename T>
    static T clamp(T value, T min, T max) {
        if (value > max) {
            return max;
        }
        if (min > value) {
            return min;
        }
        return value;
    }

    template<typename T>
    static T clamp01(T value) {
        return clamp<T>(value, 0, 1);
    }

    void PathRunningSystem::step(un::World& world, f32 delta, un::JobWorker* worker) {
        for (auto[entity, path, translation] : world.view<un::Path, un::Translation>()
                                                    .each()) {
            float newPos = path.position + (delta * path.speed);
            auto& positions = path.positions;
            int nPoints = positions.size();
            if (newPos > nPoints) {
                newPos -= nPoints;
            }
            path.position = newPos;
            float time = fmod(newPos, 1);
            int index = floor(newPos);
            int next = (index + 1) % nPoints;
            translation.value = lerp(positions[index], positions[next], time);
        }
    }

    void PathRunningSystem::describe(SystemDescriptor& descriptor) {
        descriptor.dependsOn<un::TransformSystem>();
    }

}