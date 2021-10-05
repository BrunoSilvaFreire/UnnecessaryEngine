#include "gameplay.h"
#include <unnecessary/components/dummy.h>
#include <unnecessary/components/common.h>
#include <unnecessary/systems/transform.h>

#define DEG2RAD (M_PI * 2) / 360
namespace un {
    template<typename T>
    static constexpr T lerp(T x, T y, float t) {
        return (x + (y - x) * t);
    }

    glm::vec3 forward(const Rotation& rotation) {
        return glm::vec3(0, 0, 1) * rotation.value;
    }

    glm::vec3 right(const Rotation& rotation) {
        return glm::vec3(1, 0, 0) * rotation.value;
    }

    glm::vec3 up(const Rotation& rotation) {
        return glm::vec3(0, 1, 0) * rotation.value;
    }

    template<typename T>
    static float inv_lerp(T x, T y, T value) {
        if (x < 0) {
            y -= x;
            value -= x;
            x = 0;
        }
        return (value - x) / (y - x);
    }

    static bool approximately(float a, float b);

    template<typename T>
    static T clamp(T value, T min, T max) {
        if (value > max) {
            return max;
        }
        if (min > value) {
            return min;
        }
        return value;
    }

    template<typename T>
    static T clamp01(T value) {
        return clamp<T>(value, 0, 1);
    }

    void PathRunningSystem::step(un::World& world, f32 delta, un::JobWorker* worker) {
        for (auto[entity, path, translation]: world.view<un::Path, un::Translation>()
                                                   .each()) {
            float newPos = path.position + (delta * path.speed);
            auto& positions = path.positions;
            int nPoints = positions.size();
            if (nPoints == 0) {
                continue;
            }
            if (newPos > nPoints) {
                newPos -= nPoints;
            }
            path.position = newPos;
            float time = fmod(newPos, 1);
            int index = floor(newPos);
            int next = (index + 1) % nPoints;
            translation.value = lerp(positions[index], positions[next], time);
        }
    }

    void PathRunningSystem::describe(SystemDescriptor& descriptor) {
        descriptor.dependsOn<un::TransformSystem>();
    }

    double getInput(GLFWwindow* window, int32_t positive, int32_t negative) {
        return (glfwGetKey(window, positive) - glfwGetKey(window, negative));
    }

    void FreeFlightSystem::step(World& world, f32 dt, un::JobWorker* worker) {
        for (auto[e, translation, rot, nav]: world.view<un::Translation, un::Rotation, un::FreeFlight>()
                                                  .each()) {
            auto speed = nav.speed;
            glm::vec3 fwd(0, 0, 1);
            glm::vec3 up(0, 1, 0);
            glm::vec3 right(1, 0, 0);
            bool reset = glfwGetKey(wnd, GLFW_KEY_R);
            if (reset) {
                translation.value = glm::vec3(0, 0, -10);
            }
            if (reset) {
                rot.value = glm::quat(glm::vec3(0, 0, 0));
            }
            double newMouseX, newMouseY;
            glfwGetCursorPos(wnd, &newMouseX, &newMouseY);
            auto dX = newMouseX - lastMouseX;
            auto dY = newMouseY - lastMouseY;
            lastMouseX = newMouseX;
            lastMouseY = newMouseY;

            auto yaw =
                (dY + getInput(wnd, GLFW_KEY_LEFT, GLFW_KEY_RIGHT)) * nav.angularSpeed *
                dt;
            auto pitch =
                (dX + getInput(wnd, GLFW_KEY_UP, GLFW_KEY_DOWN)) * nav.angularSpeed * dt;

            rot.value = glm::quat(glm::vec3(-yaw, 0, 0)) * rot.value *
                        glm::quat(glm::vec3(0, pitch, 0));

            fwd = un::forward(rot);
            up = un::up(rot);
            right = un::right(rot);
            float fwdM = getInput(wnd, GLFW_KEY_W, GLFW_KEY_S) * speed * dt;
            float sideM = getInput(wnd, GLFW_KEY_A, GLFW_KEY_D) * speed * dt;
            float upDownM = getInput(wnd, GLFW_KEY_E, GLFW_KEY_Q) * speed * dt;
            fwd *= fwdM;
            right *= sideM;
            up *= upDownM;
            // Normalize
            auto dir = fwd + right + up;
            translation.value += dir;
        }
    }

    void FreeFlightSystem::describe(SystemDescriptor& descriptor) {
        descriptor.runsOnStage(un::kEarlyGameplay);
    }

    FreeFlightSystem::FreeFlightSystem(GLFWwindow* window) : wnd(window) {}

    void OrbitSystem::describe(SystemDescriptor& descriptor) {
        descriptor.runsOnStage(un::kEarlyGameplay);
    }

    void OrbitSystem::step(World& world, f32 delta, un::JobWorker* worker) {
        for (auto[entity, path, translation, rotation]: world.view<
            un::Orbit,
            un::Translation,
            un::Rotation
        >().each()) {
            float currentRad = path.position;
            float increment = path.speed * delta;
            currentRad += increment;
            glm::vec3 offset(
                glm::sin(currentRad) * path.radius,
                path.height,
                glm::cos(currentRad) * path.radius
            );
            glm::vec3 finalPos = path.center + offset;
            translation.value = finalPos;
            glm::vec3 dir = glm::normalize(path.center - finalPos);
            rotation.value = glm::vec3(0,currentRad- (DEG2RAD * 360), 0);
            path.position = currentRad;
        }
    }
}