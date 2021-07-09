#include <unnecessary/def.h>
#include <unnecessary/logging.h>
#include <unnecessary/graphics/rendering_device.h>
#include <unnecessary/algorithm/vulkan_requirements.h>
#include <optional>
#include <unnecessary/graphics/image.h>

namespace un {
    const vk::Device& RenderingDevice::getVirtualDevice() const {
        return virtualDevice;
    }

    const vk::PhysicalDevice& RenderingDevice::getPhysicalDevice() const {
        return physicalDevice;
    }

    const vk::SurfaceKHR& RenderingDevice::getSurface() const {
        return surface;
    }

    std::vector<vk::QueueFamilyProperties> selectQueueFamily(
            const vk::PhysicalDevice& device
    ) {
        return device.getQueueFamilyProperties();
    }

    vk::PhysicalDeviceProperties selectDeviceProperties(
            const vk::PhysicalDevice& device
    ) {
        return device.getProperties();
    }

    typedef TransformingRequirement<vk::PhysicalDevice, std::vector<vk::QueueFamilyProperties>, &selectQueueFamily>
            DeviceQueueRequirements;
    typedef TransformingRequirement<vk::PhysicalDevice, vk::PhysicalDeviceProperties, &selectDeviceProperties>
            DevicePropertiesRequirements;

    vk::DeviceQueueCreateInfo makeQueueCreateInfoFor(
            vk::QueueFlags flags,
            std::vector<vk::QueueFamilyProperties>& queueProperties
    ) {
        float priority = 1.0;

        for (u32 i = 0; i < queueProperties.size(); ++i) {
            auto property = queueProperties[i];
            if ((property.queueFlags & flags) != flags) {
                continue;
            }
            return vk::DeviceQueueCreateInfo((vk::DeviceQueueCreateFlags) 0, i, 1, &priority);
        }
        throw std::runtime_error("Unable to find queue for given flags");
    }

    RenderingDevice::RenderingDevice() = default;

    RenderingDevice::RenderingDevice(
            const vk::Instance& instance,
            GLFWwindow* window
    ) {

        VkSurfaceKHR tempSurface;

        vkCall((vk::Result) glfwCreateWindowSurface(
                instance,
                window,
                nullptr,
                &tempSurface
        ));
        surface = tempSurface;
        auto devices = instance.enumeratePhysicalDevices();
        un::CompositeRequirement<vk::PhysicalDevice> deviceRequirements;

        auto& queueRequirements = deviceRequirements.emplace<un::DeviceQueueRequirements>();

        queueRequirements.emplace<un::VulkanQueueAvailableRequirement>(vk::QueueFlagBits::eGraphics);
        queueRequirements.emplace<un::VulkanQueueAvailableRequirement>(vk::QueueFlagBits::eTransfer);

        auto& propertiesRequirements = deviceRequirements.emplace<un::DevicePropertiesRequirements>();
        propertiesRequirements.emplace<VulkanDeviceTypeRequirement>(vk::PhysicalDeviceType::eDiscreteGpu);


        std::unordered_map<u32, u32> scores;
        for (u32 i = 0; i < devices.size(); ++i) {
            const vk::PhysicalDevice& device = devices[i];
            vk::PhysicalDeviceProperties properties = device.getProperties();
            if (!deviceRequirements.isMet(device)) {
                LOG(INFO) << "Device " << RED(properties.deviceName) << " is not suitable.";
                //Not suitable
                continue;
            }
            LOG(INFO) << "Found candidate device " << GREEN(properties.deviceName) << ".";
            std::vector<vk::QueueFamilyProperties> queueProperties = device.getQueueFamilyProperties();
            u32 score = 0;
            //TODO: Scoring
            scores[i] = score;
        }
        typedef typename std::unordered_map<u32, u32>::value_type pair_type;
        auto electedIndex = std::max_element(
                std::begin(scores), std::end(scores),
                [](const pair_type& p1, const pair_type& p2) {
                    return p1.second < p2.second;
                }
        );
        vk::PhysicalDevice elected = devices[electedIndex->first];
        u32 score = electedIndex->second;
        LOG(INFO) << "Elected " << GREEN(elected.getProperties().deviceName) << " with score: " << PURPLE(score);
        physicalDevice = elected;
        vk::DeviceCreateInfo deviceCreateInfo;
        std::vector<vk::QueueFamilyProperties> queueProperties = physicalDevice.getQueueFamilyProperties();
        auto graphicsInfo = makeQueueCreateInfoFor(vk::QueueFlagBits::eGraphics, queueProperties);
        float priority = 1.0;

        std::optional<u32> graphicsQueue, presentQueue;

        for (u32 i = 0; i < queueProperties.size(); ++i) {
            auto property = queueProperties[i];
            if ((property.queueFlags & vk::QueueFlagBits::eGraphics) != vk::QueueFlagBits::eGraphics) {
                continue;
            }
            if (!graphicsQueue.has_value()) {
                graphicsQueue = i;
            }
            if (!presentQueue.has_value() && physicalDevice.getSurfaceSupportKHR(i, tempSurface)) {
                presentQueue = i;
            }
        }
        std::vector<vk::DeviceQueueCreateInfo> queuesToCreate;
        queuesToCreate.emplace_back((vk::DeviceQueueCreateFlags) 0, graphicsQueue.value(), 1, &priority);
        bool isSame = graphicsQueue.value() == presentQueue.value();
        if (!isSame) {
            queuesToCreate.emplace_back((vk::DeviceQueueCreateFlags) 0, presentQueue.value(), 1, &priority);
        }
        deviceCreateInfo.setQueueCreateInfos(queuesToCreate);
        std::vector<const char*> deviceExtensions;
        deviceExtensions.emplace_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
        deviceCreateInfo.setPEnabledExtensionNames(deviceExtensions);
        virtualDevice = physicalDevice.createDevice(deviceCreateInfo);
        graphics = Queue(
                graphicsInfo.queueFamilyIndex,
                virtualDevice.getQueue(graphicsInfo.queueFamilyIndex, 0)
        );

        if (isSame) {
            present = graphics;
        } else {
            present = Queue(
                    presentQueue.value(),
                    virtualDevice.getQueue(presentQueue.value(), 0)
            );
        }
        memoryProperties = physicalDevice.getMemoryProperties();
    }

    u32 RenderingDevice::selectMemoryTypeFor(
            const vk::MemoryRequirements& requirements,
            vk::MemoryPropertyFlags flags
    ) const {
        u32 typeFilter = requirements.memoryTypeBits;
        for (u32 i = 0; i < memoryProperties.memoryTypeCount; ++i) {
            vk::MemoryType type = memoryProperties.memoryTypes[i];
            if ((type.propertyFlags & flags) != flags) {
                continue;
            }
            if (typeFilter & (1 << i)) {
                return i;
            }
        }
        return -1;
    }

    Queue& RenderingDevice::getGraphics() {
        return graphics;
    }

    Queue& RenderingDevice::getPresent() {
        return present;
    }
}