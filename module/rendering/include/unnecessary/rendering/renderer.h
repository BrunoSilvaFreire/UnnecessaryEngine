#ifndef UNNECESSARYENGINE_RENDERER_H
#define UNNECESSARYENGINE_RENDERER_H
#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>
#include <glm/glm.hpp>
#include <unnecessary/version.h>
#include <unnecessary/logging.h>
#include <unnecessary/rendering/rendering_device.h>
#include <unnecessary/rendering/swapchain.h>
#include <unnecessary/rendering/window.h>
#include <unnecessary/rendering/render_graph.h>
#include <unnecessary/rendering/rendering_pipeline.h>

#ifdef UN_VULKAN_DEBUG

#include <unnecessary/rendering/debug/vulkan_debugger.h>

#endif
namespace un {

    class Renderer {
    private:
        un::Window* _window;
        vk::Instance _vulkan;
        un::RenderingDevice _device;
        un::SwapChain _swapChain;
        un::RenderGraph _graph;
#ifdef UN_VULKAN_DEBUG
        un::VulkanDebugger _debugger;
#endif
    public:

        Renderer(Window* window, std::string string, Version version);

        void usePipeline(un::RenderingPipeline* pipeline) {
            pipeline->configure(_graph);
            _graph.bake(*this);
        }

        un::Window* getWindow() const {
            return _window;
        }

        const vk::Instance& getVulkan() const {
            return _vulkan;
        }

        vk::Device getVirtualDevice() const {
            return _device.getVirtualDevice();
        }

        vk::PhysicalDevice getPhysicalDevice() const {
            return _device.getPhysicalDevice();
        }

        const RenderingDevice& getDevice() const;

        const un::RenderGraph& getRenderGraph() const {
            return _graph;
        }

        const VulkanDebugger& getDebugger() const;

        const un::SwapChain& getSwapChain() const;

        un::SwapChain& getSwapChain();

#ifdef DEBUG

        template<typename ValueType>
        UN_AGGRESSIVE_INLINE void tag(
            ValueType value,
            const std::string& tagName
        ) {
            _debugger.tag<ValueType>(value, tagName);
        }

#endif

    };
}
#endif
