#include <unnecessary/def.h>
#include <unnecessary/logging.h>
#include <unnecessary/rendering/rendering_device.h>
#include <unnecessary/rendering/vulkan/vulkan_requirements.h>
#include <unnecessary/rendering/vulkan/vulkan_utils.h>
#include <optional>

namespace un {
    vk::DeviceMemory allocate_memory_for(
        const RenderingDevice& device,
        vk::MemoryPropertyFlags flags,
        vk::MemoryRequirements requirements
    ) {
        return device.get_virtual_device().allocateMemory(
            vk::MemoryAllocateInfo(
                requirements.size,
                device.select_memory_type_for(requirements, flags)
            )
        );
    }

    const vk::Device& rendering_device::get_virtual_device() const {
        return _virtualDevice;
    }

    const vk::PhysicalDevice& rendering_device::get_physical_device() const {
        return _physicalDevice;
    }

    const vk::SurfaceKHR& rendering_device::get_surface() const {
        return _surface;
    }

    std::vector<vk::QueueFamilyProperties> select_queue_family(
        const vk::PhysicalDevice& device
    ) {
        return device.getQueueFamilyProperties();
    }

    vk::PhysicalDeviceProperties select_device_properties(
        const vk::PhysicalDevice& device
    ) {
        return device.getProperties();
    }

    using device_queue_requirements = transforming_requirement<
        vk::PhysicalDevice, std::vector<vk::QueueFamilyProperties>, &select_queue_family
    >;
    using device_properties_requirements = transforming_requirement<
        vk::PhysicalDevice, vk::PhysicalDeviceProperties, &select_device_properties
    >;

    vk::DeviceQueueCreateInfo make_queue_create_info_for(
        vk::QueueFlags flags,
        std::vector<vk::QueueFamilyProperties>& queueProperties
    ) {
        float priority = 1.0;

        for (u32 i = 0; i < queueProperties.size(); ++i) {
            auto property = queueProperties[i];
            if ((property.queueFlags & flags) != flags) {
                continue;
            }
            return vk::DeviceQueueCreateInfo(
                static_cast<vk::DeviceQueueCreateFlags>(0),
                i,
                1,
                &priority
            );
        }
        throw std::runtime_error("Unable to find queue for given flags");
    }

    RenderingDevice rendering_device::create(
        const vk::Instance& instance,
        GLFWwindow* window
    ) {
        vk::SurfaceKHR surface;
        VK_CALL(
            static_cast<vk::Result>(
                glfwCreateWindowSurface(
                    instance,
                    window,
                    nullptr,
                    reinterpret_cast<VkSurfaceKHR*>(&surface)
                )
            )
        );
        auto devices = instance.enumeratePhysicalDevices();
        composite_requirement<vk::PhysicalDevice> deviceRequirements;

        auto& queueRequirements = deviceRequirements.emplace<device_queue_requirements>();

        queueRequirements.emplace<vulkan_queue_available_requirement>(vk::QueueFlagBits::eGraphics);
        queueRequirements.emplace<vulkan_queue_available_requirement>(vk::QueueFlagBits::eTransfer);

        auto& propertiesRequirements = deviceRequirements.emplace<device_properties_requirements>();
        propertiesRequirements.emplace<vulkan_device_type_requirement>(
            std::initializer_list<vk::PhysicalDeviceType>(
                {
                    vk::PhysicalDeviceType::eDiscreteGpu,
                    vk::PhysicalDeviceType::eIntegratedGpu
                }
            )
        );

        std::unordered_map<u32, u32> scores;
        for (u32 i = 0; i < devices.size(); ++i) {
            const vk::PhysicalDevice& device = devices[i];
            vk::PhysicalDeviceProperties properties = device.getProperties();
            validator validator;
            deviceRequirements.check(device, validator);
            if (!validator.is_valid()) {
                LOG(INFO) << "Device " << RED(properties.deviceName) << " is not suitable.";
                const auto& problems = validator.get_problems();
                LOG(INFO) << "Problems (" << problems.size() << "):";
                for (const problem& problem : problems) {
                    LOG(INFO) << RED(problem.get_description());
                }
                //Not suitable
                continue;
            }
            LOG(INFO) << "Found candidate renderer "
                      << GREEN(properties.deviceName)
                      << ".";
            std::vector<vk::QueueFamilyProperties> queueProperties = device.getQueueFamilyProperties();
            u32 score = 0;
            //TODO: Scoring
            scores[i] = score;
        }
        using pair_type = std::unordered_map<u32, u32>::value_type;
        auto electedIndex = std::max_element(
            std::begin(scores), std::end(scores),
            [](const pair_type& p1, const pair_type& p2) {
                return p1.second < p2.second;
            }
        );
        vk::PhysicalDevice elected = devices[electedIndex->first];
        u32 score = electedIndex->second;

        auto physicalDevice = elected;
        auto deviceProperties = physicalDevice.getProperties();
        LOG(INFO) << "Elected " << GREEN(deviceProperties.deviceName) << " with score: "
                  << PURPLE(score);
        vk::DeviceCreateInfo deviceCreateInfo;
        std::vector<vk::QueueFamilyProperties> queueProperties = physicalDevice.getQueueFamilyProperties();
        auto graphicsInfo = make_queue_create_info_for(
            vk::QueueFlagBits::eGraphics,
            queueProperties
        );
        float priority = 1.0;

        std::optional<u32> graphicsQueue, presentQueue;

        for (u32 i = 0; i < queueProperties.size(); ++i) {
            auto property = queueProperties[i];
            if ((property.queueFlags & vk::QueueFlagBits::eGraphics) !=
                vk::QueueFlagBits::eGraphics) {
                continue;
            }
            if (!graphicsQueue.has_value()) {
                graphicsQueue = i;
            }
            if (!presentQueue.has_value() &&
                physicalDevice.getSurfaceSupportKHR(i, surface)) {
                presentQueue = i;
            }
        }
        std::vector<vk::DeviceQueueCreateInfo> queuesToCreate;
        queuesToCreate.emplace_back(
            static_cast<vk::DeviceQueueCreateFlags>(0),
            graphicsQueue.value(),
            1,
            &priority
        );
        bool isSame = graphicsQueue.value() == presentQueue.value();
        if (!isSame) {
            queuesToCreate.emplace_back(
                static_cast<vk::DeviceQueueCreateFlags>(0),
                presentQueue.value(),
                1,
                &priority
            );
        }
        deviceCreateInfo.setQueueCreateInfos(queuesToCreate);
        std::vector<const char*> deviceExtensions;
        deviceExtensions.emplace_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
        for (const char* extension : deviceExtensions) {
            LOG(INFO) << "Using device extension: " << GREEN(extension);
        }
        deviceCreateInfo.setPEnabledExtensionNames(deviceExtensions);
        vk::PhysicalDeviceFeatures features;
        //        features.geometryShader = true;
        deviceCreateInfo.setPEnabledFeatures(&features);
        auto virtualDevice = physicalDevice.createDevice(deviceCreateInfo);
        auto graphics = queue(
            graphicsInfo.queueFamilyIndex,
            virtualDevice.getQueue(graphicsInfo.queueFamilyIndex, 0)
        );
        auto memoryProperties = physicalDevice.getMemoryProperties();
        if (isSame) {
            return rendering_device(
                virtualDevice,
                physicalDevice,
                surface,
                graphics,
                graphics,
                memoryProperties,
                deviceProperties
            );
        }
        return rendering_device(
            virtualDevice,
            physicalDevice,
            surface,
            graphics,
            queue(
                presentQueue.value(),
                virtualDevice.getQueue(presentQueue.value(), 0)
            ),
            memoryProperties,
            deviceProperties
        );
    }

    u32 rendering_device::select_memory_type_for(
        const vk::MemoryRequirements& requirements,
        vk::MemoryPropertyFlags flags
    ) const {
        u32 typeFilter = requirements.memoryTypeBits;
        for (u32 i = 0; i < _memoryProperties.memoryTypeCount; ++i) {
            vk::MemoryType type = _memoryProperties.memoryTypes[i];
            if ((type.propertyFlags & flags) != flags) {
                continue;
            }
            if (typeFilter & (1 << i)) {
                return i;
            }
        }
        return -1;
    }

    queue& rendering_device::get_graphics() {
        return _graphics;
    }

    const queue& rendering_device::get_graphics() const {
        return _graphics;
    }

    queue& rendering_device::get_present() {
        return present;
    }

    const vk::PhysicalDeviceMemoryProperties& rendering_device::get_memory_properties() const {
        return _memoryProperties;
    }

    const vk::PhysicalDeviceProperties& rendering_device::get_device_properties() const {
        return _deviceProperties;
    }

    rendering_device::rendering_device(
        const vk::Device& virtualDevice,
        const vk::PhysicalDevice& physicalDevice,
        const vk::SurfaceKHR& surface,
        const queue& present,
        const queue& graphics,
        const vk::PhysicalDeviceMemoryProperties& memoryProperties,
        const vk::PhysicalDeviceProperties& deviceProperties
    ) : _virtualDevice(virtualDevice),
        _physicalDevice(physicalDevice),
        _surface(surface),
        present(present),
        _graphics(graphics),
        _memoryProperties(memoryProperties),
        _deviceProperties(deviceProperties) {
    }

    const queue& rendering_device::get_present() const {
        return present;
    }
}
