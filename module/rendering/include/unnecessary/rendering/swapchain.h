#ifndef UNNECESSARYENGINE_SWAPCHAIN_H
#define UNNECESSARYENGINE_SWAPCHAIN_H

#include <vector>
#include <vulkan/vulkan.hpp>
#include <unnecessary/rendering/window.h>
#include <unnecessary/rendering/image_view.h>
#include <unnecessary/rendering/rendering_device.h>

namespace un {

    class SwapChainSupportDetails {
    private:
        vk::SurfaceCapabilitiesKHR capabilities;
        std::vector<vk::SurfaceFormatKHR> formats;
        std::vector<vk::PresentModeKHR> presentModes;
    public:
        SwapChainSupportDetails(
            const un::RenderingDevice& renderingDevice,
            vk::ImageUsageFlags usageFlags,
            vk::ImageCreateFlags createFlags
        );

        const vk::SurfaceCapabilitiesKHR& getCapabilities() const;

        vk::SurfaceFormatKHR selectFormat();

        vk::Extent2D selectExtent(const un::Size2D& application);

        vk::PresentModeKHR selectPresentMode();
    };


    class SwapChain {
    public:
        struct ChainImage {
        private:
            vk::Image image;
            un::ImageView imageView;
        public:
            ChainImage(const vk::Image& image, const un::ImageView& imageView);

            const vk::Image& getImage() const;

            const un::ImageView& getImageView() const;
        };

        struct ChainSynchronizer {
            vk::Semaphore imageReady;
            vk::Semaphore renderFinished;
            vk::Fence fence;
            bool submitted;
        };

    private:
        vk::Format format;
        vk::SwapchainKHR swapChain;
        un::Size2D resolution;
        std::vector<ChainImage> images;
        std::size_t semaphoreIndex;
        std::vector<ChainSynchronizer> synchonizers;
    public:


        SwapChain() = default;

        SwapChain(
            un::RenderingDevice& renderingDevice,
            const un::Size2D& targetSize
        );

        vk::Format getFormat() const;

        const vk::SwapchainKHR& getSwapChain() const;

        const Size2D& getResolution() const;

        ChainSynchronizer acquireSynchronizer();

        const std::vector<ChainImage>& getImages() const;

        std::size_t getNumLinks();
    };
}
#endif
