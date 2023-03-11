#include <unnecessary/rendering/renderer.h>

namespace un {
    vk::Instance create_vulkan_instance(
        std::string appName,
        version appVersion
    ) {
        vk::ApplicationInfo appInfo(
            appName.c_str(),
            VK_MAKE_VERSION(appVersion.get_major(),
                            appVersion.get_minor(),
                            appVersion.get_patch()),
            "UnnecessaryEngine",
            VK_MAKE_VERSION(0, 1, 0),
            VK_MAKE_VERSION(1, 2, 0)
        );
        std::vector<const char*> instanceLayers;
        std::vector<const char*> instanceExtensions;
#ifdef DEBUG
        LOG(INFO) << "Using validation layers";
        instanceLayers.emplace_back("VK_LAYER_KHRONOS_validation");
        //        instanceLayers.emplace_back("VK_LAYER_LUNARG_api_dump");
        instanceExtensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif
        u32 count;
        const char** requiredExtensions = glfwGetRequiredInstanceExtensions(&count);
        for (u32 i = 0; i < count; ++i) {
            instanceExtensions.emplace_back(requiredExtensions[i]);
        }
        auto layers = vk::enumerateInstanceLayerProperties();
        auto extensions = vk::enumerateInstanceExtensionProperties();
        for (const auto& layer : layers) {
            LOG(INFO) << "Found layer " << GREEN(layer.layerName);
        }
        return createInstance(
            vk::InstanceCreateInfo(
                static_cast<vk::InstanceCreateFlags>(0),
                &appInfo,
                instanceLayers,
                instanceExtensions
            )
        );
    }

    renderer::renderer(
        window* window,
        std::string string,
        version version
    ) :
        _vulkan(create_vulkan_instance(string, version)),
        _device(rendering_device::create(_vulkan, window->get_window())),
#ifdef UN_VULKAN_DEBUG
        _debugger(*this),
#endif
        _swapChain(*this, window->get_window_size()),
        _window(window) {
        tag(_device.get_virtual_device(), "VirtualDevice");
        const vk::Queue& graphicsQueue = *_device.get_graphics();
        const vk::Queue& presentQueue = *_device.get_present();
        if (graphicsQueue != presentQueue) {
            tag(graphicsQueue, "GraphicsQueue");
            tag(presentQueue, "PresentQueue");
        }
        else {
            tag(graphicsQueue, "Graphics&PresentQueue");
        }
    }

    const rendering_device& renderer::get_device() const {
        return _device;
    }

    const swap_chain& renderer::get_swap_chain() const {
        return _swapChain;
    }

    swap_chain& renderer::get_swap_chain() {
        return _swapChain;
    }

#if DEBUG

    const vulkan_debugger& renderer::get_debugger() const {
        return _debugger;
    }

#endif
}
