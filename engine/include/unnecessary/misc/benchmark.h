
#ifndef UNNECESSARYENGINE_BENCHMARK_H
#define UNNECESSARYENGINE_BENCHMARK_H

#include <chrono>

namespace un {
    template<class Resolution = std::chrono::milliseconds>
    class Chronometer {
    public:
        using Clock = std::conditional_t<std::chrono::high_resolution_clock::is_steady,
                std::chrono::high_resolution_clock,
                std::chrono::steady_clock>;

    private:
        const Clock::time_point start = Clock::now();

    public:
        Chronometer() = default;

        inline size_t stop() const {
            const auto end = Clock::now();
            return std::chrono::duration_cast<Resolution>(end - start).count();
        }
    };
}
#endif //UNNECESSARYENGINE_BENCHMARK_H
