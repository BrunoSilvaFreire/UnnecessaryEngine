//
// Created by brunorbsf on 24/08/2021.
//

#ifndef UNNECESSARYENGINE_GAMEPLAY_H
#define UNNECESSARYENGINE_GAMEPLAY_H

#include <unnecessary/systems/system.h>
#include <GLFW/glfw3.h>
namespace un {
    class PathRunningSystem : public un::System {
    public:
        void step(World &world, f32 delta, un::JobWorker *worker) override;

        void describe(SystemDescriptor &descriptor) override;
    };

    struct FreeFlight {
        float speed;
        float angularSpeed;
    };

    class FreeFlightSystem : public un::System {
    private:
        GLFWwindow* wnd;
        float lastMouseX, lastMouseY;
    public:
        FreeFlightSystem(GLFWwindow *window);

        void step(World &world, f32 delta, un::JobWorker *worker) override;
    };
}


#endif //UNNECESSARYENGINE_GAMEPLAY_H
