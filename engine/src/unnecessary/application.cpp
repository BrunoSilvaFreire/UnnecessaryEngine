#include <unnecessary/application.h>
#include <unordered_set>
#include <functional>

namespace un {
    template<typename T>
    void assertHasElements(
        const std::string& name,
        std::vector<T> elements,
        std::vector<const char*>& requirements,
        const std::function<const char*(const T&)>& selector
    ) {
        std::vector<std::string> notFound;
        for (const char*& requirement : requirements) {
            bool found = false;
            for (const T& property : elements) {
                std::string propertyName = std::string(selector(property));


                if (propertyName == requirement) {
                    LOG(INFO) << "Found " << name << ' ' << propertyName;
                    found = true;
                    break;
                }

            }
            if (!found) {
                notFound.emplace_back(requirement);
            }
        }
        if (!notFound.empty()) {
            for (std::string& layer:notFound) {
                LOG(FUCK) << name << " '" << layer << "' not found.";
            }
            throw std::runtime_error("Unable to find all required elements.");
        }
    }

    vk::Instance loadVulkan(
        const std::string& name,
        const Version& appVersion
    ) {
        std::vector<const char*> instanceExtensions;
        std::vector<const char*> layers;
        if (glfwInit() != GLFW_TRUE) {
            throw std::runtime_error("Unable to initialize GLFW");
        }
        auto vulkanSupportCode = glfwVulkanSupported();
        if (vulkanSupportCode != GLFW_TRUE) {
            std::string msg = "Vulkan not supported by runtime ";
            msg += "(";
            msg += std::to_string(vulkanSupportCode);
            msg += ")";
            throw std::runtime_error(msg);
        }

        u32 count;
        const char** requiredExtensions = glfwGetRequiredInstanceExtensions(&count);
        for (u32 i = 0; i < count; ++i) {
            instanceExtensions.emplace_back(requiredExtensions[i]);
        }
#ifdef DEBUG
        LOG(INFO) << "Using validation layers";
        layers.emplace_back("VK_LAYER_KHRONOS_validation");
#endif
        assertHasElements<vk::ExtensionProperties>(
            "extension",
            vk::enumerateInstanceExtensionProperties(),
            instanceExtensions,
            [](const vk::ExtensionProperties& properties) {
                return properties.extensionName.data();
            }
        );

        assertHasElements<vk::LayerProperties>(
            "layer",
            vk::enumerateInstanceLayerProperties(),
            layers,
            [](const vk::LayerProperties& properties) {
                return properties.layerName.data();
            }
        );

        vk::ApplicationInfo appInfo(
            name.c_str(),
            VK_MAKE_VERSION(appVersion.getMajor(),
                            appVersion.getMinor(),
                            appVersion.getMinor()),
            "Unnecessary Engine",
            VK_MAKE_VERSION(0, 1, 0),
            VK_MAKE_VERSION(1, 0, 0)
        );
        vk::InstanceCreateInfo info(
            (vk::InstanceCreateFlags) 0,
            &appInfo,
            layers,
            instanceExtensions
        );
        try {
            return vk::createInstance(info);

        } catch (vk::LayerNotPresentError& layerNotPresentError) {
            LOG(FUCK) << RED(
                layerNotPresentError.what() << " (" << layerNotPresentError.code()
                                            << ")");
            throw layerNotPresentError;
        }
    }

    Application::Application(
        const std::string& name,
        const Version& version,
        int nThreads
    ) : name(name),
        version(version),
        pooling(true),
        vulkan(
            loadVulkan(
                name,
                version
            )),
        renderer(nullptr),
        jobSystem(nullptr) {
        LOG(INFO) << "Initializing app " << GREEN(name);

        monitor = glfwGetPrimaryMonitor();
        vidMode = glfwGetVideoMode(monitor);
        bool fullscreen = false;
        GLFWmonitor* mon;
        if (fullscreen) {
            glfwWindowHint(GLFW_RED_BITS, vidMode->redBits);
            glfwWindowHint(GLFW_BLUE_BITS, vidMode->blueBits);
            glfwWindowHint(GLFW_GREEN_BITS, vidMode->greenBits);
            glfwWindowHint(GLFW_REFRESH_RATE, vidMode->refreshRate);
            width = vidMode->width;
            height = vidMode->height;
            mon = monitor;
        } else {
            width = 640;
            height = 480;
            mon = nullptr;
        }
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        window = glfwCreateWindow(width, height, name.c_str(), mon, nullptr);
        glfwMakeContextCurrent(window);
        renderer = new un::Renderer(vulkan, window);
        jobSystem = new un::JobSystem(*this, nThreads);
    }

    void Application::execute() {
        while (pooling) {
            glfwPollEvents();
            onPool(1.0F / 60);
            pooling = !glfwWindowShouldClose(window);
        }
    }

    const vk::Instance& Application::getVulkan() const {
        return vulkan;
    }

    Renderer& Application::getRenderer() {
        return *renderer;
    }

    const std::string& Application::getName() const {
        return name;
    }

    const Version& Application::getVersion() const {
        return version;
    }

    GLFWwindow* Application::getWindow() const {
        return window;
    }

    Event<f32>& Application::getOnPool() {
        return onPool;
    }

    const GLFWvidmode* const Application::getVidMode() const {
        return vidMode;
    }

    GLFWmonitor* Application::getMonitor() const {
        return monitor;
    }

    Application::~Application() {
        delete renderer;
    }

    JobSystem& Application::getJobSystem() {
        return *jobSystem;
    }

    u32 Application::getWidth() const {
        return width;
    }

    u32 Application::getHeight() const {
        return height;
    }
}