#include <unnecessary/application/application.h>

namespace un {
    void app_loop::late(event_void::listener listener) {
        _lateStep += listener;
    }

    void app_loop::step() {
        _earlyStep();
        _lateStep();
    }

    void app_loop::early(event_void::listener listener) {
        _earlyStep += listener;
    }

    const app_loop& application::get_fixed_loop() const {
        return _fixedLoop;
    }

    const app_loop& application::get_variable_loop() const {
        return _variableLoop;
    }

    app_loop& application::get_fixed_loop() {
        return _fixedLoop;
    }

    app_loop& application::get_variable_loop() {
        return _variableLoop;
    }

    void application::wait_exit() {
    }

    float application::get_fixed_frame_rate() const {
        return _fixedFrameRate;
    }

    void application::set_fixed_frame_rate(float fixedFrameRate) {
        _fixedFrameRate = fixedFrameRate;
    }

    const event_void& application::get_on_start() const {
        return _onStart;
    }

    const event_void& application::get_on_stop() const {
        return _onStop;
    }

    event_void& application::get_on_stop() {
        return _onStop;
    }

    event_void& application::get_on_start() {
        return _onStart;
    }

    void application::stop() {
        _onStop();
        _running = false;
    }
}
