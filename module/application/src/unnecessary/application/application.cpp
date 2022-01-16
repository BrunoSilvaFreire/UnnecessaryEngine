#include <unnecessary/application/application.h>

namespace un {
    void AppLoop::late(un::EventVoid::EventListener listener) {
        lateStep += listener;
    }

    void AppLoop::step() {
        earlyStep();
        lateStep();
    }

    void AppLoop::early(un::EventVoid::EventListener listener) {
        earlyStep += listener;
    }

    const AppLoop& Application::getFixedLoop() const {
        return fixedLoop;
    }

    const AppLoop& Application::getVariableLoop() const {
        return variableLoop;
    }

    AppLoop& Application::getFixedLoop() {
        return fixedLoop;
    }

    AppLoop& Application::getVariableLoop() {
        return variableLoop;
    }

    void Application::waitExit() {

    }

    float Application::getFixedFrameRate() const {
        return fixedFrameRate;
    }

    void Application::setFixedFrameRate(float fixedFrameRate) {
        Application::fixedFrameRate = fixedFrameRate;
    }

    const EventVoid& Application::getOnStart() const {
        return onStart;
    }

    const EventVoid& Application::getOnStop() const {
        return onStop;
    }

    un::EventVoid& Application::getOnStop() {
        return onStop;
    }

    un::EventVoid& Application::getOnStart() {
        return onStart;
    }
}