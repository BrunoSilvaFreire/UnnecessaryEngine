#ifndef UNNECESSARYENGINE_LOOP_H
#define UNNECESSARYENGINE_LOOP_H

#include <chrono>
#include <thread>

namespace un {
    class SteadyLoop {
    private:
        float frequency = 60;
        std::chrono::high_resolution_clock::time_point startTime;
        std::chrono::high_resolution_clock::time_point endTime;

    public:
        float getFrequency() const {
            return frequency;
        }

        void setFrequency(float frequency) {
            SteadyLoop::frequency = frequency;
        }

        std::chrono::nanoseconds getLoopTimeFrame() const {
            return std::chrono::nanoseconds(
                static_cast<std::chrono::nanoseconds::rep>(
                    1.0F / frequency * std::chrono::nanoseconds::period::den
                )
            );
        }

        void exit() {
            endTime = std::chrono::high_resolution_clock::now();
        }

        void enter() {
            auto lastLoopDuration = endTime - startTime;
            std::chrono::nanoseconds sleepDuration =
                getLoopTimeFrame() - lastLoopDuration;
            if (sleepDuration.count() > 0) {
                LOG(INFO) << "Sleeping for " << sleepDuration;
                std::this_thread::sleep_for(sleepDuration);
            }
            startTime = std::chrono::high_resolution_clock::now();
        }
    };
}
#endif
