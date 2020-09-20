#include <unnecessary/application.h>
#include <unnecessary/logging.h>
#include <unordered_set>

namespace un {
    vk::Instance loadVulkan(
            const std::string &name,
            const Version &appVersion
    ) {
        std::vector<const char *> instanceExtensions;
        std::vector<const char *> layers;
        u32 count;
        if (glfwInit() != GLFW_TRUE) {
            throw std::runtime_error("Unable to initialize GLFW");
        }
        if (glfwVulkanSupported() != GLFW_TRUE) {
            throw std::runtime_error("Vulkan not supported by runtime");
        }
        const char **requiredExtensions = glfwGetRequiredInstanceExtensions(&count);
        for (u32 i = 0; i < count; ++i) {
            instanceExtensions.emplace_back(requiredExtensions[i]);
        }
#ifdef DEBUG
        layers.emplace_back("VK_LAYER_KHRONOS_validation");
#endif
        vk::ApplicationInfo appInfo(
                name.c_str(),
                VK_MAKE_VERSION(appVersion.getMajor(), appVersion.getMinor(), appVersion.getMinor()),
                "Unnecessary Engine",
                VK_MAKE_VERSION(0, 1, 0),
                VK_MAKE_VERSION(1, 0, 0)
        );
        vk::InstanceCreateInfo info(
                (vk::InstanceCreateFlags) 0,
                &appInfo
        );
        info.setPEnabledExtensionNames(instanceExtensions);
        info.setPEnabledLayerNames(layers);
        return vk::createInstance(info);
    }

    Application::Application(
            const std::string &name,
            const Version &version
    ) : name(name),
        version(version),
        pooling(true),
        vulkan(loadVulkan(name, version)),
        renderer(vulkan) {
        LOG(INFO) << "Initializing app " << GREEN(name);

        auto monitor = glfwGetPrimaryMonitor();
        auto mode = glfwGetVideoMode(monitor);
        glfwWindowHint(GLFW_RED_BITS, mode->redBits);
        glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
        glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
        glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
        window = glfwCreateWindow(mode->width, mode->height, name.c_str(), monitor, nullptr);
    }

    void Application::execute() {
        while (pooling) {
            glfwPollEvents();
            onPool(1.0F / 60);
            pooling = !glfwWindowShouldClose(window);
        }
    }

    const vk::Instance &Application::getVulkan() const {
        return vulkan;
    }

    Renderer &Application::getRenderer() {
        return renderer;
    }

    const std::string &Application::getName() const {
        return name;
    }

    const Version &Application::getVersion() const {
        return version;
    }

    GLFWwindow *Application::getWindow() const {
        return window;
    }

    Event<f32> &Application::getOnPool() {
        return onPool;
    }
}