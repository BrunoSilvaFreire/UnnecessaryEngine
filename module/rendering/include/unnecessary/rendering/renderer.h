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

namespace un {

    class Renderer {
    private:
        un::Window* _window;
        vk::Instance _vulkan;
        un::RenderingDevice _device;
        un::SwapChain _swapChain;
        un::RenderGraph _graph;
    public:

        Renderer(Window* window, std::string string, Version version);

        void usePipeline(un::RenderingPipeline* pipeline) {
            pipeline->configure(_graph);
            _graph.bake(_device.getVirtualDevice());
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

        const SwapChain& getSwapChain() const;
    };
}
#endif
