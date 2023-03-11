#include <unnecessary/rendering/swapchain.h>
#include <unnecessary/rendering/renderer.h>
#include <unnecessary/logging.h>

namespace un {
    swap_chain::swap_chain(
        const renderer& renderer,
        const size2d& targetSize
    ) {
        auto renderingDevice = renderer.get_device();
        vk::ImageUsageFlags swapChainUsageFlags =
            vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eStorage;
        auto swapChainCreationFlags = static_cast<vk::ImageCreateFlags>(0);
        SwapChainSupportDetails details(
            renderingDevice,
            swapChainUsageFlags,
            swapChainCreationFlags
        );
        auto selectedFormat = details.select_format();
        _format = selectedFormat._format;
        vk::Extent2D extent = details.select_extent(targetSize);
        _resolution = size2d(extent.width, extent.height);
        auto& capabilities = details.get_capabilities();
        u32 imageCount = std::min<u32>(capabilities.minImageCount + 1, 3);
        u32 maxImageCount = capabilities.maxImageCount;
        if (maxImageCount > 0 && imageCount > maxImageCount) {
            imageCount = maxImageCount;
        }

        const vk::SurfaceKHR& surface = renderingDevice.get_surface();
        vk::SwapchainCreateInfoKHR swapChainInfo{};
        swapChainInfo.setSurface(surface);
        swapChainInfo.setMinImageCount(imageCount);
        swapChainInfo.setImageFormat(_format);
        swapChainInfo.setImageColorSpace(selectedFormat.colorSpace);
        swapChainInfo.setImageExtent(extent);
        swapChainInfo.setImageArrayLayers(1);
        swapChainInfo.setImageUsage(swapChainUsageFlags);
        /*    (vk::SwapchainCreateFlagsKHR) vk::SwapchainCreateFlagBitsKHR::eMutableFormat,
            surface,
            imageCount,
            format,
            selectedFormat.colorSpace,
            extent,
            1,
            swapChainUsageFlags
        );*/
        swapChainInfo.setPresentMode(details.select_present_mode());

        u32 graphicsQueueIndex = renderingDevice.get_graphics().get_index();
        u32 presentQueueIndex = renderingDevice.get_present().get_index();

        std::vector<u32> indices;
        if (graphicsQueueIndex != presentQueueIndex) {
            swapChainInfo.setImageSharingMode(vk::SharingMode::eConcurrent);
            indices.push_back(graphicsQueueIndex);
            indices.push_back(presentQueueIndex);
        }
        else {
            swapChainInfo.setImageSharingMode(vk::SharingMode::eExclusive);
            indices.push_back(graphicsQueueIndex);
        }
        swapChainInfo.setQueueFamilyIndices(indices);
        swapChainInfo.setPreTransform(capabilities.currentTransform);
        swapChainInfo.setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque);
        swapChainInfo.setClipped(true);
        const vk::Device& device = renderingDevice.get_virtual_device();
        VK_CALL(device.createSwapchainKHR(&swapChainInfo, nullptr, &_swapChain));
        auto createdImages = device.getSwapchainImagesKHR(_swapChain);
        std::size_t nImages = createdImages.size();
        _images.reserve(nImages);
        _synchronizers.resize(nImages);
        for (int i = 0; i < nImages; ++i) {
            const chain_image& image = _images.emplace_back(
                createdImages[i],
                image_view(
                    renderingDevice,
                    createdImages[i],
                    _format
                )
            );

            std::string suffix = "-";
            suffix += std::to_string(i);
            std::string imagePrefix = "ChainImage-";
            renderer.tag(image.get_image(), imagePrefix + "Image" + suffix);
            renderer.tag(*image.get_image_view(), imagePrefix + "ImageView" + suffix);
            std::unique_ptr<chain_synchronizer>& ptr = _synchronizers[i];
            ptr = std::make_unique<chain_synchronizer>(device);
#ifdef DEBUG
            ptr->set_inner_index(i);
#endif
            std::string syncronizerPrefix = "ChainSynchronizer-";

            renderer.tag(
                ptr->get_image_ready(),
                syncronizerPrefix + "ImageReadySemaphore" + suffix
            );
            renderer.tag(
                ptr->get_render_finished(),
                syncronizerPrefix + "RenderFinishedSemaphore" + suffix
            );
            renderer.tag(ptr->get_fence(), syncronizerPrefix + "Fence" + suffix);
        }
        _semaphoreIndex = 0;
    }

    vk::Format swap_chain::get_format() const {
        return _format;
    }

    const vk::SwapchainKHR& swap_chain::get_swap_chain() const {
        return _swapChain;
    }

    const size2d& swap_chain::get_resolution() const {
        return _resolution;
    }

    swap_chain::chain_synchronizer& swap_chain::acquire_synchronizer() {
        std::size_t index = _semaphoreIndex++;
        _semaphoreIndex %= _synchronizers.size();
        return *_synchronizers[index];
    }

    const std::vector<swap_chain::chain_image>& swap_chain::get_images() const {
        return _images;
    }

    std::size_t swap_chain::get_num_links() {
        return _images.size();
    }

    vk::SurfaceFormatKHR swap_chain_support_details::select_format() {
        std::vector<vk::SurfaceFormatKHR> preferences;
        preferences.emplace_back(
            vk::Format::eB8G8R8A8Srgb,
            vk::ColorSpaceKHR::eSrgbNonlinear
        );
        for (const vk::SurfaceFormatKHR preference : preferences) {
            for (const auto& availableFormat : _formats) {
                if (availableFormat.format == preference.format &&
                    availableFormat.colorSpace == preference.colorSpace) {
                    return availableFormat;
                }
            }
        }
        return _formats[0];
    }

    const vk::SurfaceCapabilitiesKHR& swap_chain_support_details::get_capabilities() const {
        return _capabilities;
    }

    swap_chain_support_details::swap_chain_support_details(
        const rendering_device& renderingDevice,
        vk::ImageUsageFlags usageFlags,
        vk::ImageCreateFlags createFlags
    ) {
        auto& device = renderingDevice.get_physical_device();
        auto& surface = renderingDevice.get_surface();
        _capabilities = device.getSurfaceCapabilitiesKHR(surface);

        for (const auto& item : device.getSurfaceFormatsKHR(surface)) {
            vk::ImageFormatProperties properties;
            auto result = device.getImageFormatProperties(
                item.format,
                vk::ImageType::e2D,
                vk::ImageTiling::eOptimal,
                usageFlags,
                createFlags,
                &properties
            );
            if (result == vk::Result::eSuccess) {
                _formats.push_back(item);
            }
        }
        _presentModes = device.getSurfacePresentModesKHR(surface);
    }

    vk::Extent2D swap_chain_support_details::select_extent(const size2d& application) {
        if (_capabilities.currentExtent.width != UINT32_MAX) {
            return _capabilities.currentExtent;
        }
        VkExtent2D actualExtent = {
            static_cast<uint32_t>(application.x),
            static_cast<uint32_t>(application.y)
        };

        actualExtent.width = std::max(
            _capabilities.minImageExtent.width,
            std::min(_capabilities.maxImageExtent.width, actualExtent.width));
        actualExtent.height = std::max(
            _capabilities.minImageExtent.height,
            std::min(_capabilities.maxImageExtent.height, actualExtent.height));

        return actualExtent;
    }

    vk::PresentModeKHR swap_chain_support_details::select_present_mode() {
        for (const auto& availablePresentMode : _presentModes) {
            if (availablePresentMode == vk::PresentModeKHR::eMailbox) {
                return availablePresentMode;
            }
        }

        return vk::PresentModeKHR::eFifo;
    }

    swap_chain::chain_image::chain_image(
        const vk::Image& image,
        const image_view& imageView
    ) : _image(image), _imageView(imageView) {
    }

    const vk::Image& swap_chain::chain_image::get_image() const {
        return _image;
    }

    const image_view& swap_chain::chain_image::get_image_view() const {
        return _imageView;
    }

    swap_chain::chain_synchronizer::chain_synchronizer(
        vk::Device device
    ) : _inUse(false) {
        _imageReady = device.createSemaphore(
            vk::SemaphoreCreateInfo(
                static_cast<vk::SemaphoreCreateFlags>(0)
            )
        );
        _renderFinished = device.createSemaphore(
            vk::SemaphoreCreateInfo(
                static_cast<vk::SemaphoreCreateFlags>(0)
            )
        );
        _fence = device.createFence(
            vk::FenceCreateInfo(
                vk::FenceCreateFlagBits::eSignaled
            )
        );
    }

    void swap_chain::chain_synchronizer::access() {
        std::unique_lock guard(_mutex);
        if (_inUse) {
            _available.wait(guard);
        }
        _inUse = true;
    }

    void swap_chain::chain_synchronizer::unlock() {
        std::lock_guard<std::mutex> guard(_mutex);
        _available.notify_one();
        _inUse = false;
    }

    const vk::Semaphore& swap_chain::chain_synchronizer::get_image_ready() const {
        return _imageReady;
    }

    const vk::Semaphore& swap_chain::chain_synchronizer::get_render_finished() const {
        return _renderFinished;
    }

    const vk::Fence& swap_chain::chain_synchronizer::get_fence() const {
        return _fence;
    }

#ifdef DEBUG

    size_t swap_chain::chain_synchronizer::get_inner_index() const {
        return _innerIndex;
    }

    void swap_chain::chain_synchronizer::set_inner_index(size_t innerIndex) {
        chain_synchronizer::_innerIndex = innerIndex;
    }

#endif
}
