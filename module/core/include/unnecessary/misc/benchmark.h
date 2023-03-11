#ifndef UNNECESSARYENGINE_BENCHMARK_H
#define UNNECESSARYENGINE_BENCHMARK_H

#include <chrono>
#include <type_traits>
#include <cstddef>

namespace un {
    template<class Resolution = std::chrono::milliseconds>
    class chronometer {
    public:
        using clock = std::conditional_t<
            std::chrono::high_resolution_clock::is_steady,
            std::chrono::high_resolution_clock,
            std::chrono::steady_clock
        >;

    private:
        clock::time_point _start;

    public:
        chronometer(bool autoStart = true) {
            if (autoStart) {
                start();
            }
        };

        void start() {
            _start = clock::now();
        }

        clock::time_point get_start() const {
            return _start;
        }

        Resolution stop() const {
            const auto end = clock::now();
            return std::chrono::duration_cast<Resolution>(end - _start);
        }
    };
}
#endif //UNNECESSARYENGINE_BENCHMARK_H
