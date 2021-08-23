#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>
#include <unnecessary/application.h>
#include <unnecessary/graphics/renderer.h>
#include <unnecessary/algorithm/vulkan_requirements.h>
#include <unnecessary/def.h>
#include <unnecessary/logging.h>
#include <algorithm>
#include <optional>
#include <unordered_map>

namespace un {

    const vk::PhysicalDevice& Renderer::getPhysicalDevice() const {
        return renderingDevice.getPhysicalDevice();
    }

    const vk::Device& Renderer::getVirtualDevice() const {
        return renderingDevice.getVirtualDevice();
    }


    Queue& Renderer::getGraphics() {
        return renderingDevice.getGraphics();
    }

    const Queue& Renderer::getGraphics() const {
        return renderingDevice.getGraphics();
    }


    Renderer::Renderer(
        const vk::Instance& instance,
        GLFWwindow* window
    ) : renderingDevice(instance, window) {
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);

        swapChain = un::SwapChain(
            renderingDevice,
            un::Size2D(width, height)
        );
        vk::CommandPoolCreateInfo poolInfo(
            (vk::CommandPoolCreateFlags) vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
            renderingDevice.getGraphics().getIndex()
        );
        vkCall(renderingDevice.getVirtualDevice()
                              .createCommandPool(&poolInfo, nullptr, &globalPool));
    }


    vk::CommandPool& Renderer::getGlobalPool() {
        return globalPool;
    }

    un::SwapChain& Renderer::getSwapChain() {
        return swapChain;
    }

    RenderingDevice& Renderer::getRenderingDevice() {
        return renderingDevice;
    }

    RenderingPipeline* Renderer::getCurrentPipeline() const {
        return currentPipeline;
    }
}