#include <unnecessary/graphics/swapchain.h>
#include <unnecessary/application.h>
#include <unnecessary/graphics/lighting.h>

namespace un {

    SwapChain::SwapChain(
        un::RenderingDevice& renderingDevice,
        const un::Size2D& targetSize
    ) {
        vk::ImageUsageFlags swapChainUsageFlags =
            vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eStorage;
        auto swapChainCreationFlags = static_cast<vk::ImageCreateFlags>(0);
        SwapChainSupportDetails details(
            renderingDevice,
            swapChainUsageFlags,
            swapChainCreationFlags
        );
        auto selectedFormat = details.selectFormat();
        format = selectedFormat.format;
        vk::Extent2D extent = details.selectExtent(targetSize);
        resolution = un::Size2D(extent.width, extent.height);
        auto& capabilities = details.getCapabilities();
        u32 imageCount = capabilities.minImageCount + 1;
        u32 maxImageCount = capabilities.maxImageCount;
        if (maxImageCount > 0 && imageCount > maxImageCount) {
            imageCount = maxImageCount;
        }

        const vk::SurfaceKHR& surface = renderingDevice.getSurface();
        vk::SwapchainCreateInfoKHR swapChainInfo(
            (vk::SwapchainCreateFlagsKHR) 0,
            surface,
            imageCount,
            format,
            selectedFormat.colorSpace,
            extent,
            1,
            swapChainUsageFlags
        );
        swapChainInfo.setPresentMode(details.selectPresentMode());

        u32 graphicsQueueIndex = renderingDevice.getGraphics().getIndex();
        u32 presentQueueIndex = renderingDevice.getPresent().getIndex();

        if (graphicsQueueIndex != presentQueueIndex) {
            swapChainInfo.setImageSharingMode(vk::SharingMode::eConcurrent);
            std::array<u32, 2> indices({graphicsQueueIndex, presentQueueIndex});
            swapChainInfo.setQueueFamilyIndices(indices);
        } else {
            swapChainInfo.setImageSharingMode(vk::SharingMode::eExclusive);
            std::array<u32, 1> indices({graphicsQueueIndex});
            swapChainInfo.setQueueFamilyIndices(indices);
        }
        swapChainInfo.setPreTransform(capabilities.currentTransform);
        swapChainInfo.setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque);
        swapChainInfo.setClipped(true);
        const vk::Device& device = renderingDevice.getVirtualDevice();
        vkCall(device.createSwapchainKHR(&swapChainInfo, nullptr, &swapChain));
        images = device.getSwapchainImagesKHR(swapChain);
        views.reserve(images.size());
        for (int i = 0; i < images.size(); ++i) {
            views.emplace_back(
                renderingDevice,
                images[i],
                format
            );
        }
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

    const std::vector<un::ImageView>& SwapChain::getViews() const {
        return views;
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
}