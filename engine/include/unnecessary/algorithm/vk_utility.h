#ifndef UNNECESSARYENGINE_VK_UTILITY_H
#define UNNECESSARYENGINE_VK_UTILITY_H

#include <vulkan/vulkan.hpp>
#include <unnecessary/graphics/rendering_device.h>

namespace un {

    vk::DeviceMemory allocateMemoryFor(
        const un::RenderingDevice& device,
        vk::MemoryPropertyFlags flags,
        vk::MemoryRequirements requirements
    );

    template<typename T>
    bool tryFindFunction(const vk::Instance& vulkan, const char* name, T** result) {
        return (*result = reinterpret_cast<T*>(vulkan.getProcAddr(name))) != nullptr;
    }

    template<typename T>
    void loadFunction(const vk::Instance& vulkan, const char* name, T** result) {
        if (!tryFindFunction(vulkan, name, result)) {
            std::stringstream err;
            err << "Unable to find vulkan function '";
            err << name;
            err << "'.";
            throw std::runtime_error(err.str());
        }
    }

    class VulkanFunctionDatabase {
    private:
        PFN_vkSetDebugUtilsObjectNameEXT pSetDebugUtilsObjectName{};


    public:
        VulkanFunctionDatabase(vk::Instance vulkan);

        void setDebugUtilsObjectName(
            vk::Device device,
            const vk::DebugUtilsObjectNameInfoEXT& nameInfo
        ) const;
    };

    template<class T>
    void tag(
        T obj,
        vk::Device device,
        const un::VulkanFunctionDatabase& database,
        const std::string& tag
    ) {
        database.setDebugUtilsObjectName(
            device,
            vk::DebugUtilsObjectNameInfoEXT(
                obj.objectType,
                (uint64_t) ((typename T::NativeType) obj),
                tag.c_str()
            )
        );
    }
}
#endif
