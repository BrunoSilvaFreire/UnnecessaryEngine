#include <unnecessary/renderer.h>
#include <unnecessary/algorithm/vulkan_requirements.h>
#include <unordered_map>
#include <unnecessary/def.h>
#include <unnecessary/logging.h>

namespace un {

    vk::PhysicalDevice &Renderer::getPhysicalDevice() {
        return physicalDevice;
    }

    vk::Device &Renderer::getVirtualDevice() {
        return virtualDevice;
    }

    std::vector<vk::QueueFamilyProperties> selectQueueFamily(const vk::PhysicalDevice &device) {
        return device.getQueueFamilyProperties();
    }

    vk::PhysicalDeviceProperties selectDeviceProperties(const vk::PhysicalDevice &device) {
        return device.getProperties();
    }

    typedef TransformingRequirement<vk::PhysicalDevice, std::vector<vk::QueueFamilyProperties>, &selectQueueFamily> DeviceQueueRequirements;
    typedef TransformingRequirement<vk::PhysicalDevice, vk::PhysicalDeviceProperties, &selectDeviceProperties> DevicePropertiesRequirements;

    Renderer::Renderer(vk::Instance instance) {
        auto devices = instance.enumeratePhysicalDevices();
        CompositeRequirement<vk::PhysicalDevice> deviceRequirements;

        auto &queueRequirements = deviceRequirements.emplace<DeviceQueueRequirements>();
        queueRequirements.emplace<VulkanQueueAvailableRequirement>(vk::QueueFlagBits::eGraphics);

        auto &propertiesRequirements = deviceRequirements.emplace<DevicePropertiesRequirements>();
        propertiesRequirements.emplace<VulkanDeviceTypeRequirement>(vk::PhysicalDeviceType::eDiscreteGpu);


        std::unordered_map<vk::PhysicalDevice, u32> scores;

        for (const vk::PhysicalDevice &device : devices) {
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
            scores[device] = score;
        }
        typedef std::unordered_map<vk::PhysicalDevice, u32>::value_type pair_type;
        auto elected = std::max_element(
                std::begin(scores), std::end(scores),
                [](const pair_type &p1, const pair_type &p2) {
                    return p1.second < p2.second;
                }
        );
        LOG(INFO) << "Elected " << GREEN(elected->first.getProperties().deviceName) << " @ " << PURPLE(elected->second);
        physicalDevice = elected->first;
        vk::DeviceCreateInfo deviceCreateInfo;
        vk::DeviceQueueCreateInfo graphicsQueue;
        auto queueProperties = physicalDevice.getQueueFamilyProperties();
        float priority = 1.0;
        for (u32 i = 0; i < queueProperties.size(); ++i) {
            auto property = queueProperties[i];
            if ((property.queueFlags & vk::QueueFlagBits::eGraphics) != vk::QueueFlagBits::eGraphics) {
                continue;
            }
            graphicsQueue = vk::DeviceQueueCreateInfo((vk::DeviceQueueCreateFlags) 0, i, 1, &priority);
            break;
        }
        deviceCreateInfo.setPQueueCreateInfos(&graphicsQueue);
        deviceCreateInfo.setQueueCreateInfoCount(1);
        virtualDevice = physicalDevice.createDevice(deviceCreateInfo);
        graphics = virtualDevice.getQueue(graphicsQueue.queueFamilyIndex, 0);
    }
}