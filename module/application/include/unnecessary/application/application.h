#ifndef UNNECESSARYENGINE_APPLICATION_H
#define UNNECESSARYENGINE_APPLICATION_H

#include <unnecessary/misc/event.h>

namespace un {
    class AppLoop {
    private:
        un::EventVoid earlyStep;
        un::EventVoid lateStep;

    public:

        void early(un::EventVoid::EventListener listener);

        void late(un::EventVoid::EventListener listener);

        void step();
    };

    class Application;

    class Extension {
    public:
        virtual void apply(un::Application& application) = 0;
    };

    class Application {
    private:
        un::AppLoop fixedLoop;
        un::AppLoop variableLoop;
        un::EventVoid onStart, onStop;
        bool running;
        float fixedFrameRate = 60;
    public:
        float getFixedFrameRate() const;

        void setFixedFrameRate(float fixedFrameRate);

        const un::AppLoop& getFixedLoop() const;

        const un::AppLoop& getVariableLoop() const;

        un::AppLoop& getFixedLoop();

        un::AppLoop& getVariableLoop();

        void start() {
            running = true;
            onStart();
            while (running) {
                fixedLoop.step();
            }
        }

        void waitExit();

        const EventVoid& getOnStart() const;

        const EventVoid& getOnStop() const;


        un::EventVoid& getOnStart();

        un::EventVoid& getOnStop();

        template<typename E>
        void extend(E& extension) {
            extension.apply(*this);
        }
    };
}
#endif
