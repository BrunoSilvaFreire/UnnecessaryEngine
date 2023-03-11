#ifndef UNNECESSARYENGINE_APPLICATION_H
#define UNNECESSARYENGINE_APPLICATION_H

#include <unnecessary/extension.h>
#include <unnecessary/misc/event.h>

namespace un {
    class app_loop {
    private:
        event_void _earlyStep;
        event_void _lateStep;

    public:
        void early(event_void::listener listener);

        void late(event_void::listener listener);

        void step();
    };

    class application;

    using app_extension = extension<application>;

    class application {
    private:
        app_loop _fixedLoop;
        app_loop _variableLoop;
        event_void _onStart, _onStop;
        bool _running = false;
        float _fixedFrameRate = 60;

    public:
        float get_fixed_frame_rate() const;

        void set_fixed_frame_rate(float fixedFrameRate);

        const app_loop& get_fixed_loop() const;

        const app_loop& get_variable_loop() const;

        app_loop& get_fixed_loop();

        app_loop& get_variable_loop();

        void start() {
            _running = true;
            _onStart();
            while (_running) {
                _fixedLoop.step();
                _variableLoop.step();
            }
        }

        void wait_exit();

        const event_void& get_on_start() const;

        const event_void& get_on_stop() const;

        event_void& get_on_start();

        event_void& get_on_stop();

        template<typename E>
        void extend(E& extension) {
            extension.apply(*this);
        }

        void stop();
    };
}
#endif
