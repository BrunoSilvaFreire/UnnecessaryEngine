#ifndef UNNECESSARYENGINE_SWAPCHAIN_H
#define UNNECESSARYENGINE_SWAPCHAIN_H

#include <vector>
#include <condition_variable>
#include <vulkan/vulkan.hpp>
#include <unnecessary/rendering/window.h>
#include <unnecessary/rendering/image_view.h>
#include <unnecessary/rendering/rendering_device.h>

namespace un {
    class renderer;

    class swap_chain_support_details {
    private:
        vk::SurfaceCapabilitiesKHR _capabilities;
        std::vector<vk::SurfaceFormatKHR> _formats;
        std::vector<vk::PresentModeKHR> _presentModes;

    public:
        swap_chain_support_details(
            const rendering_device& renderingDevice,
            vk::ImageUsageFlags usageFlags,
            vk::ImageCreateFlags createFlags
        );

        const vk::SurfaceCapabilitiesKHR& get_capabilities() const;

        vk::SurfaceFormatKHR select_format();

        vk::Extent2D select_extent(const size2d& application);

        vk::PresentModeKHR select_present_mode();
    };

    class swap_chain {
    public:
        struct chain_image {
        private:
            vk::Image _image;
            image_view _imageView;

        public:
            chain_image(const vk::Image& image, const image_view& imageView);

            const vk::Image& get_image() const;

            const image_view& get_image_view() const;
        };

        class chain_synchronizer {
        private:
            vk::Semaphore _imageReady;
            vk::Semaphore _renderFinished;
            vk::Fence _fence;
            bool _inUse;
            std::mutex _mutex;
            std::condition_variable _available;

        public:
            chain_synchronizer(vk::Device device);

            void access();

            void unlock();

            const vk::Semaphore& get_image_ready() const;

            const vk::Semaphore& get_render_finished() const;

            const vk::Fence& get_fence() const;

#ifdef DEBUG

        private:
            std::size_t _innerIndex;

        public:
            size_t get_inner_index() const;

            void set_inner_index(size_t innerIndex);

#endif
        };

    private:
        vk::Format _format;
        vk::SwapchainKHR _swapChain;
        size2d _resolution;
        std::vector<chain_image> _images;
        std::size_t _semaphoreIndex;
        std::vector<std::unique_ptr<chain_synchronizer>> _synchronizers;

    public:
        swap_chain() = default;

        swap_chain(
            const renderer& renderer,
            const size2d& targetSize
        );

        vk::Format get_format() const;

        const vk::SwapchainKHR& get_swap_chain() const;

        const size2d& get_resolution() const;

        chain_synchronizer& acquire_synchronizer();

        const std::vector<chain_image>& get_images() const;

        std::size_t get_num_links();
    };
}
#endif
