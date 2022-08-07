
#ifndef UNNECESSARYENGINE_BENCHMARK_H
#define UNNECESSARYENGINE_BENCHMARK_H

#include <chrono>
#include <type_traits>
#include <cstddef>

namespace un {
    template<class Resolution = std::chrono::milliseconds>
    class Chronometer {
    public:
        using Clock = std::conditional_t<std::chrono::high_resolution_clock::is_steady,
            std::chrono::high_resolution_clock,
            std::chrono::steady_clock>;

    private:
        Clock::time_point _start;

    public:
        Chronometer(bool autoStart = true) {
            if (autoStart) {
                start();
            }
        };

        void start() {
            _start = Clock::now();
        }

        Clock::time_point getStart() const {
            return _start;
        }

        Resolution stop() const {
            const auto end = Clock::now();
            return std::chrono::duration_cast<Resolution>(end - _start);
        }
    };
}
#endif //UNNECESSARYENGINE_BENCHMARK_H
