#ifndef UNNECESSARYENGINE_APPLICATION_H
#define UNNECESSARYENGINE_APPLICATION_H

#include <string>

#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>
#include <unnecessary/version.h>
#include <unnecessary/graphics/renderer.h>
#include <unnecessary/algorithm/event.h>

namespace un {
    class Application {
    private:
        vk::Instance vulkan;
        Renderer renderer;
        std::string name;
        Version version;
        GLFWwindow *window;
        bool pooling;
        Event<f32> onPool;

    public:
        Application(const std::string &name, const Version &version);

        void execute();

        const vk::Instance &getVulkan() const;

        Renderer &getRenderer();

        const std::string &getName() const;

        const Version &getVersion() const;

        GLFWwindow *getWindow() const;

        Event<f32> &getOnPool();
    };
};
#endif
