//
// Created by brunorbsf on 24/08/2021.
//

#ifndef UNNECESSARYENGINE_GAMEPLAY_H
#define UNNECESSARYENGINE_GAMEPLAY_H

#include <GLFW/glfw3.h>
#include <unnecessary/systems/system.h>
#include <unnecessary/jobs/parallel_for_job.h>
#include <unnecessary/components/common.h>
#include <unnecessary/components/common.h>
#include <unnecessary/systems/transform.h>

namespace un {

    class PathRunningSystem : public un::SimpleSystem {
    public:
        void step(World& world, f32 delta, un::job_worker* worker) override;

        void describe(SystemDescriptor& descriptor) override;
    };

    class OrbitSystem : public un::SimpleSystem {
    public:
        void describe(SystemDescriptor& descriptor) override;

        void step(World& world, f32 delta, un::job_worker* worker) override;
    };

    struct FreeFlight {
        float speed;
        float angularSpeed;
    };

    class FreeFlightSystem : public un::SimpleSystem {
    private:
        GLFWwindow* wnd;
        float lastMouseX, lastMouseY;
    public:
        FreeFlightSystem(GLFWwindow* window);

        void step(World& world, f32 delta, un::job_worker* worker) override;

        void describe(SystemDescriptor& descriptor) override;
    };
}


#endif //UNNECESSARYENGINE_GAMEPLAY_H
