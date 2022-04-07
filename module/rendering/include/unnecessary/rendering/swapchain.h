#ifndef UNNECESSARYENGINE_SWAPCHAIN_H
#define UNNECESSARYENGINE_SWAPCHAIN_H

#include <vector>
#include <condition_variable>
#include <vulkan/vulkan.hpp>
#include <unnecessary/rendering/window.h>
#include <unnecessary/rendering/image_view.h>
#include <unnecessary/rendering/rendering_device.h>

namespace un {
    class Renderer;

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

        class ChainSynchronizer {
        private:
            vk::Semaphore imageReady;
            vk::Semaphore renderFinished;
            vk::Fence fence;
            bool inUse;
            std::mutex _mutex;
            std::condition_variable available;
        public:
            ChainSynchronizer(vk::Device device);

            void access();

            void unlock();

            const vk::Semaphore& getImageReady() const;

            const vk::Semaphore& getRenderFinished() const;

            const vk::Fence& getFence() const;

#ifdef DEBUG
        private:
            std::size_t innerIndex;
        public:
            size_t getInnerIndex() const;

            void setInnerIndex(size_t innerIndex);

#endif
        };

    private:
        vk::Format format;
        vk::SwapchainKHR swapChain;
        un::Size2D resolution;
        std::vector<ChainImage> images;
        std::size_t semaphoreIndex;
        std::vector<std::unique_ptr<ChainSynchronizer>> synchonizers;
    public:


        SwapChain() = default;

        SwapChain(
            const un::Renderer& renderer,
            const un::Size2D& targetSize
        );

        vk::Format getFormat() const;

        const vk::SwapchainKHR& getSwapChain() const;

        const Size2D& getResolution() const;

        ChainSynchronizer& acquireSynchronizer();

        const std::vector<ChainImage>& getImages() const;

        std::size_t getNumLinks();
    };
}
#endif
