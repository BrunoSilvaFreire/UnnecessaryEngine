#ifndef UNNECESSARYENGINE_LOOP_H
#define UNNECESSARYENGINE_LOOP_H

#include <chrono>
#include <thread>

namespace un {
    class steady_loop {
    private:
        float _frequency = 60;
        std::chrono::high_resolution_clock::time_point _startTime;
        std::chrono::high_resolution_clock::time_point _endTime;

    public:
        float get_frequency() const {
            return _frequency;
        }

        void set_frequency(float frequency) {
            steady_loop::_frequency = frequency;
        }

        std::chrono::nanoseconds get_loop_time_frame() const {
            return std::chrono::nanoseconds(
                static_cast<std::chrono::nanoseconds::rep>(
                    1.0F / _frequency * std::chrono::nanoseconds::period::den
                )
            );
        }

        void exit() {
            _endTime = std::chrono::high_resolution_clock::now();
        }

        void enter() {
            auto lastLoopDuration = _endTime - _startTime;
            std::chrono::nanoseconds timeFrame = get_loop_time_frame();
            std::chrono::nanoseconds sleepDuration = timeFrame - lastLoopDuration;
            if (sleepDuration.count() > 0) {
                auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(sleepDuration);
                std::this_thread::sleep_for(sleepDuration);
            }
            _startTime = std::chrono::high_resolution_clock::now();
        }
    };
}
#endif
