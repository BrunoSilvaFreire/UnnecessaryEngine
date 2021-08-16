#ifndef UNNECESSARYENGINE_APPLICATION_H
#define UNNECESSARYENGINE_APPLICATION_H

#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>
#include <string>
#include <vulkan/vulkan.hpp>
#include <unnecessary/version.h>
#include <unnecessary/graphics/renderer.h>
#include <unnecessary/algorithm/event.h>
#include <unnecessary/jobs/jobs.h>

namespace un {

    class Application {
    private:
        vk::Instance vulkan;
        Renderer* renderer;
        std::string name;
        Version version;
        GLFWwindow* window;
        const GLFWvidmode* vidMode;
        GLFWmonitor* monitor;
        bool pooling;
        Event<f32> onPool;
        JobSystem* jobSystem;
        u32 width, height;

    public:
        Application(const std::string& name, const Version& version, int nThreads = -1);

        virtual ~Application();

        void execute();

        u32 getWidth() const;

        u32 getHeight() const;

        const vk::Instance& getVulkan() const;

        Renderer& getRenderer();

        const std::string& getName() const;

        const Version& getVersion() const;

        GLFWwindow* getWindow() const;

        const GLFWvidmode* const getVidMode() const;

        GLFWmonitor* getMonitor() const;

        Event<f32>& getOnPool();

        JobSystem& getJobSystem();
    };
};
#endif
