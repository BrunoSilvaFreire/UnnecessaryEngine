#ifndef UNNECESSARYENGINE_SWAPCHAIN_H
#define UNNECESSARYENGINE_SWAPCHAIN_H

#include <unnecessary/def.h>
#include <unnecessary/graphics/image_view.h>
#include <vulkan/vulkan.hpp>
#include <vector>

namespace un {

    class Renderer;

    class SwapChainSupportDetails {
    private:
        vk::SurfaceCapabilitiesKHR capabilities;
        std::vector<vk::SurfaceFormatKHR> formats;
        std::vector<vk::PresentModeKHR> presentModes;
    public:
        SwapChainSupportDetails(
                const un::RenderingDevice &renderingDevice
        );

        const vk::SurfaceCapabilitiesKHR &getCapabilities() const;

        vk::SurfaceFormatKHR selectFormat();

        vk::Extent2D selectExtent(const un::Size2D &application);

        vk::PresentModeKHR selectPresentMode();
    };

    class SwapChain {
    private:
        vk::Format format;
        vk::SwapchainKHR swapChain;
        un::Size2D resolution;
        std::vector<un::ImageView> views;
    public:
        SwapChain() = default;

        SwapChain(
                un::RenderingDevice &renderingDevice,
                const un::Size2D &targetSize
        );

        vk::Format getFormat() const;

        const vk::SwapchainKHR &getSwapChain() const;

        const Size2D &getResolution() const;

        const std::vector<un::ImageView> &getViews() const;
    };
}
#endif
