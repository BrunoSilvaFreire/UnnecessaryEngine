#include <unnecessary/rendering/renderer.h>

namespace un {
    vk::Instance createVulkanInstance(
        std::string appName,
        Version appVersion
    ) {
        vk::ApplicationInfo appInfo(
            appName.c_str(),
            VK_MAKE_VERSION(appVersion.getMajor(),
                            appVersion.getMinor(),
                            appVersion.getPatch()),
            "UnnecessaryEngine",
            VK_MAKE_VERSION(0, 1, 0),
            VK_MAKE_VERSION(1, 2, 0)
        );
        std::vector<const char*> instanceLayers;
        std::vector<const char*> instanceExtensions;
#ifdef DEBUG
        LOG(INFO) << "Using validation layers";
        instanceLayers.emplace_back("VK_LAYER_KHRONOS_validation");
        instanceExtensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        instanceExtensions.emplace_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
#endif
        u32 count;
        const char** requiredExtensions = glfwGetRequiredInstanceExtensions(&count);
        for (u32 i = 0; i < count; ++i) {
            instanceExtensions.emplace_back(requiredExtensions[i]);
        }
        return vk::createInstance(
            vk::InstanceCreateInfo(
                static_cast<vk::InstanceCreateFlags>(0),
                &appInfo,
                instanceLayers,
                instanceExtensions
            )
        );
    }

    un::Renderer::Renderer(
        Window* window,
        std::string string,
        Version version
    ) :
        _vulkan(un::createVulkanInstance(string, version)),
        _device(un::RenderingDevice::create(_vulkan, window->getWindow())),
        _swapChain(_device, window->getWindowSize()),
        _window(window) {

    }

    const RenderingDevice& Renderer::getDevice() const {
        return _device;
    }

    const SwapChain& Renderer::getSwapChain() const {
        return _swapChain;
    }
}