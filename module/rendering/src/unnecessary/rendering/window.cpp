#include <unnecessary/rendering/window.h>
#include <functional>

namespace un {

    Window::Window(un::Application& application, GLFWwindow* window) : _window(window) {
        int w, h;
        glfwGetWindowSize(window, &w, &h);
        _windowSize = un::Size2D(w, h);
        application.getVariableLoop().early(
            [this]() {
                pool();
            }
        );
    }

    void Window::initGLFW() {
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

    Window Window::withSize(
        std::string title,
        un::Application& application,
        un::Size2D size
    ) {
        initGLFW();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        auto window = glfwCreateWindow(
            size.x, size.y,
            title.c_str(),
            nullptr,
            nullptr
        );
        return un::Window(application, window);
    }

    Window Window::fullscreen(
        std::string title,
        un::Application& application,
        GLFWmonitor* monitor
    ) {
        initGLFW();
        int w, h;
        glfwGetMonitorPhysicalSize(monitor, &w, &h);
        auto window = glfwCreateWindow(
            w, h,
            title.c_str(),
            monitor,
            nullptr
        );
        return un::Window(application, window);
    }

    GLFWwindow* Window::getWindow() const {
        return _window;
    }

    const Size2D& Window::getWindowSize() const {
        return _windowSize;
    }

    bool Window::shouldClose() {
        return glfwWindowShouldClose(_window);
    }

    void Window::apply(Application& application) {
        application.getVariableLoop().early([this, &application]() {
            pool();
            if (shouldClose()){
                application.stop();
            }
        });
    }
}