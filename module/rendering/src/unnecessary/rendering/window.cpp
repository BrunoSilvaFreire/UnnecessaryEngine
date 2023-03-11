#include <unnecessary/rendering/window.h>
#include <functional>

namespace un {
    window::window(application& application, GLFWwindow* window) : _window(window) {
        int w, h;
        glfwGetWindowSize(window, &w, &h);
        _windowSize = size2d(w, h);
        application.get_variable_loop().early(
            [this]() {
                pool();
            }
        );
    }

    void window::init_glfw() {
        if (!glfwInit()) {
            throw std::runtime_error("Unable to initialize glfw");
        }
        auto vulkanSupportCode = glfwVulkanSupported();
        if (vulkanSupportCode != GLFW_TRUE) {
            std::string msg = "Vulkan not supported by runtime ";
            msg += "(";
            msg += std::to_string(vulkanSupportCode);
            msg += ")";
            throw std::runtime_error(msg);
        }
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    }

    Window window::with_size(
        const std::string& title,
        application& application,
        size2d size
    ) {
        init_glfw();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        auto window = glfwCreateWindow(
            size.x, size.y,
            title.c_str(),
            nullptr,
            nullptr
        );
        return window(application, window);
    }

    un::window window::fullscreen(
        std::string title,
        application& application,
        GLFWmonitor* monitor
    ) {
        init_glfw();
        int w, h;
        glfwGetMonitorPhysicalSize(monitor, &w, &h);
        auto window = glfwCreateWindow(
            w, h,
            title.c_str(),
            monitor,
            nullptr
        );
        return un::window(application, window);
    }

    GLFWwindow* window::get_window() const {
        return _window;
    }

    const size2d& window::get_window_size() const {
        return _windowSize;
    }

    bool window::should_close() {
        return glfwWindowShouldClose(_window);
    }

    void window::apply(application& application) {
        application.get_variable_loop().early(
            [this, &application]() {
                pool();
                if (should_close()) {
                    application.stop();
                }
            }
        );
    }
}
