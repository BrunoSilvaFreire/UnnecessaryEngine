#ifndef UNNECESSARYENGINE_WINDOW_H
#define UNNECESSARYENGINE_WINDOW_H

#include <GLFW/glfw3.h>
#include <stdexcept>
#include <string>
#include <glm/glm.hpp>
#include <unnecessary/application/application.h>

namespace un {
    typedef glm::u16vec2 Size2D;

    class Window {
    private:
        GLFWwindow* _window;
        un::Size2D _windowSize;

        Window(un::Application& application, GLFWwindow* window);

        static void initGLFW();

    public:
        static Window
        withSize(std::string title, un::Application& application, un::Size2D size);

        static Window
        fullscreen(std::string title, un::Application& application, GLFWmonitor* monitor);

        GLFWwindow* getWindow() const;

        const Size2D& getWindowSize() const;

        void poolLoop() {
            do {
                pool();
            } while (!shouldClose());
        }

        void pool() {
            glfwWaitEvents();
            glfwPollEvents();
        }

        bool shouldClose();
    };
}
#endif
