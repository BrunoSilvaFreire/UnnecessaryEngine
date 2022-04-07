#include <unnecessary/rendering/swapchain.h>
#include <unnecessary/rendering/renderer.h>
#include <unnecessary/logging.h>

namespace un {

    SwapChain::SwapChain(
        const un::Renderer& renderer,
        const un::Size2D& targetSize
    ) {
        auto renderingDevice = renderer.getDevice();
        vk::ImageUsageFlags swapChainUsageFlags =
            vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eStorage;
        auto swapChainCreationFlags = static_cast<vk::ImageCreateFlags>(0);
        un::SwapChainSupportDetails details(
            renderingDevice,
            swapChainUsageFlags,
            swapChainCreationFlags
        );
        auto selectedFormat = details.selectFormat();
        format = selectedFormat.format;
        vk::Extent2D extent = details.selectExtent(targetSize);
        resolution = un::Size2D(extent.width, extent.height);
        auto& capabilities = details.getCapabilities();
        u32 imageCount = std::min<u32>(capabilities.minImageCount + 1, 3);
        u32 maxImageCount = capabilities.maxImageCount;
        if (maxImageCount > 0 && imageCount > maxImageCount) {
            imageCount = maxImageCount;
        }

        const vk::SurfaceKHR& surface = renderingDevice.getSurface();
        vk::SwapchainCreateInfoKHR swapChainInfo{};
        swapChainInfo.setSurface(surface);
        swapChainInfo.setMinImageCount(imageCount);
        swapChainInfo.setImageFormat(format);
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
        swapChainInfo.setPresentMode(details.selectPresentMode());

        u32 graphicsQueueIndex = renderingDevice.getGraphics().getIndex();
        u32 presentQueueIndex = renderingDevice.getPresent().getIndex();

        std::vector<u32> indices;
        if (graphicsQueueIndex != presentQueueIndex) {
            swapChainInfo.setImageSharingMode(vk::SharingMode::eConcurrent);
            indices.push_back(graphicsQueueIndex);
            indices.push_back(presentQueueIndex);
        } else {
            swapChainInfo.setImageSharingMode(vk::SharingMode::eExclusive);
            indices.push_back(graphicsQueueIndex);
        }
        swapChainInfo.setQueueFamilyIndices(indices);
        swapChainInfo.setPreTransform(capabilities.currentTransform);
        swapChainInfo.setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque);
        swapChainInfo.setClipped(true);
        const vk::Device& device = renderingDevice.getVirtualDevice();
        vkCall(device.createSwapchainKHR(&swapChainInfo, nullptr, &swapChain));
        auto createdImages = device.getSwapchainImagesKHR(swapChain);
        std::size_t nImages = createdImages.size();
        images.reserve(nImages);
        synchonizers.resize(nImages);
        for (int i = 0; i < nImages; ++i) {
            const ChainImage& image = images.emplace_back(
                createdImages[i],
                un::ImageView(
                    renderingDevice,
                    createdImages[i],
                    format
                )
            );

            std::string suffix = "-";
            suffix += std::to_string(i);
            std::string imagePrefix = "ChainImage-";
            renderer.tag(image.getImage(), imagePrefix + "Image" + suffix);
            renderer.tag(*image.getImageView(), imagePrefix + "ImageView" + suffix);
            std::unique_ptr<ChainSynchronizer>& ptr = synchonizers[i];
            ptr = std::make_unique<ChainSynchronizer>(device);
#ifdef DEBUG
            ptr->setInnerIndex(i);
#endif
            std::string syncronizerPrefix = "ChainSynchronizer-";

            renderer.tag(
                ptr->getImageReady(),
                syncronizerPrefix + "ImageReadySemaphore" + suffix
            );
            renderer.tag(
                ptr->getRenderFinished(),
                syncronizerPrefix + "RenderFinishedSemaphore" + suffix
            );
            renderer.tag(ptr->getFence(), syncronizerPrefix + "Fence" + suffix);
        }
        semaphoreIndex = 0;
    }

    vk::Format SwapChain::getFormat() const {
        return format;
    }

    const vk::SwapchainKHR& SwapChain::getSwapChain() const {
        return swapChain;
    }

    const Size2D& SwapChain::getResolution() const {
        return resolution;
    }

    un::SwapChain::ChainSynchronizer& SwapChain::acquireSynchronizer() {
        std::size_t index = semaphoreIndex++;
        semaphoreIndex %= synchonizers.size();
        return *synchonizers[index];
    }

    const std::vector<SwapChain::ChainImage>& SwapChain::getImages() const {
        return images;
    }

    std::size_t SwapChain::getNumLinks() {
        return images.size();
    }


    vk::SurfaceFormatKHR SwapChainSupportDetails::selectFormat() {
        std::vector<vk::SurfaceFormatKHR> preferences;
        preferences.emplace_back(
            vk::Format::eB8G8R8A8Srgb,
            vk::ColorSpaceKHR::eSrgbNonlinear
        );
        for (const vk::SurfaceFormatKHR preference : preferences) {
            for (const auto& availableFormat : formats) {
                if (availableFormat.format == preference.format &&
                    availableFormat.colorSpace == preference.colorSpace) {
                    return availableFormat;
                }
            }
        }
        return formats[0];
    }

    const vk::SurfaceCapabilitiesKHR& SwapChainSupportDetails::getCapabilities() const {
        return capabilities;
    }

    SwapChainSupportDetails::SwapChainSupportDetails(
        const un::RenderingDevice& renderingDevice,
        vk::ImageUsageFlags usageFlags,
        vk::ImageCreateFlags createFlags
    ) {
        auto& device = renderingDevice.getPhysicalDevice();
        auto& surface = renderingDevice.getSurface();
        capabilities = device.getSurfaceCapabilitiesKHR(surface);

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
                formats.push_back(item);
            }
        }
        presentModes = device.getSurfacePresentModesKHR(surface);
    }

    vk::Extent2D SwapChainSupportDetails::selectExtent(const un::Size2D& targetSize) {
        if (capabilities.currentExtent.width != UINT32_MAX) {
            return capabilities.currentExtent;
        } else {
            VkExtent2D actualExtent = {
                static_cast<uint32_t>(targetSize.x),
                static_cast<uint32_t>(targetSize.y)
            };

            actualExtent.width = std::max(
                capabilities.minImageExtent.width,
                std::min(capabilities.maxImageExtent.width, actualExtent.width));
            actualExtent.height = std::max(
                capabilities.minImageExtent.height,
                std::min(capabilities.maxImageExtent.height, actualExtent.height));

            return actualExtent;
        }
    }

    vk::PresentModeKHR SwapChainSupportDetails::selectPresentMode() {
        for (const auto& availablePresentMode : presentModes) {
            if (availablePresentMode == vk::PresentModeKHR::eMailbox) {
                return availablePresentMode;
            }
        }

        return vk::PresentModeKHR::eFifo;
    }

    SwapChain::ChainImage::ChainImage(
        const vk::Image& image,
        const ImageView& imageView
    ) : image(image), imageView(imageView) { }

    const vk::Image& SwapChain::ChainImage::getImage() const {
        return image;
    }

    const ImageView& SwapChain::ChainImage::getImageView() const {
        return imageView;
    }

    SwapChain::ChainSynchronizer::ChainSynchronizer(
        vk::Device device
    ) : inUse(false) {
        imageReady = device.createSemaphore(
            vk::SemaphoreCreateInfo(
                static_cast<vk::SemaphoreCreateFlags>(0)
            )
        );
        renderFinished = device.createSemaphore(
            vk::SemaphoreCreateInfo(
                static_cast<vk::SemaphoreCreateFlags>(0)
            )
        );
        fence = device.createFence(
            vk::FenceCreateInfo(
                vk::FenceCreateFlagBits::eSignaled
            )
        );
    }

    void SwapChain::ChainSynchronizer::access() {
        std::unique_lock guard(_mutex);
        if (inUse) {
            available.wait(guard);
        }
        inUse = true;
    }

    void SwapChain::ChainSynchronizer::unlock() {
        std::lock_guard<std::mutex> guard(_mutex);
        available.notify_one();
        inUse = false;
    }

    const vk::Semaphore& SwapChain::ChainSynchronizer::getImageReady() const {
        return imageReady;
    }

    const vk::Semaphore& SwapChain::ChainSynchronizer::getRenderFinished() const {
        return renderFinished;
    }

    const vk::Fence& SwapChain::ChainSynchronizer::getFence() const {
        return fence;
    }

#ifdef DEBUG

    size_t SwapChain::ChainSynchronizer::getInnerIndex() const {
        return innerIndex;
    }

    void SwapChain::ChainSynchronizer::setInnerIndex(size_t innerIndex) {
        ChainSynchronizer::innerIndex = innerIndex;
    }

#endif
}