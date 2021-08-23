#ifndef UNNECESSARYENGINE_RENDERER_H
#define UNNECESSARYENGINE_RENDERER_H

#include <unordered_map>
#include <glm/glm.hpp>
#include <vulkan/vulkan.hpp>
#include <unnecessary/def.h>
#include <unnecessary/graphics/queue.h>
#include <unnecessary/graphics/swapchain.h>
#include <unnecessary/graphics/shading/shader.h>
#include <unnecessary/graphics/rendering/rendering_pipeline.h>

namespace un {
    class Renderer {
    private:
        un::RenderingDevice renderingDevice;
        vk::CommandPool globalPool;
        un::SwapChain swapChain;
        un::RenderingPipeline* currentPipeline;
    public:
        explicit Renderer(
            const vk::Instance& instance,
            GLFWwindow* window
        );


        const vk::PhysicalDevice& getPhysicalDevice() const;

        const vk::Device& getVirtualDevice() const;

        un::Queue& getGraphics();

        vk::CommandPool& getGlobalPool();

        un::SwapChain& getSwapChain();

        RenderingDevice& getRenderingDevice();

        const Queue& getGraphics() const;

        template<typename T>
        T* createPipeline() {
            auto value = new T(this);
            currentPipeline = value;
            return value;
        }

        RenderingPipeline* getCurrentPipeline() const;
    };
}


#endif //UNNECESSARYENGINE_RENDERER_H
