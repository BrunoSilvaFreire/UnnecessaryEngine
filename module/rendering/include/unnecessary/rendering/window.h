#ifndef UNNECESSARYENGINE_WINDOW_H
#define UNNECESSARYENGINE_WINDOW_H

#include <GLFW/glfw3.h>
#include <stdexcept>
#include <string>
#include <glm/glm.hpp>
#include <unnecessary/application/application.h>

namespace un {
    typedef glm::u16vec2 size2d;

    class window : app_extension {
    private:
        GLFWwindow* _window;
        un::size2d _windowSize;

        window(un::application& application, GLFWwindow* window);

        static void init_glfw();

    public:
        void apply(application& application) override;

        static window
        with_size(const std::string& title, application& application, size2d size);

        static window
        fullscreen(std::string title, un::application& application, GLFWmonitor* monitor);

        GLFWwindow* get_window() const;

        const size2d& get_window_size() const;

        void pool_loop() {
            do {
                pool();
            }
            while (!should_close());
        }

        void pool() {
            //            glfwWaitEvents();
            glfwPollEvents();
        }

        bool should_close();
    };
}
#endif
