//
// Created by bruno on 20/12/2021.
//

#ifndef UNNECESSARYENGINE_VULKAN_UTILS_H
#define UNNECESSARYENGINE_VULKAN_UTILS_H

#include <unordered_map>
#include <vulkan/vulkan.hpp>

namespace un {
    void assert_vk_call(vk::Result result, const std::string& method);

    extern std::unordered_map<std::string, vk::ShaderStageFlagBits> kNameToShaderStageFlags;

    template<typename T>
    bool try_find_function(const vk::Instance& vulkan, const char* name, T** result) {
        return (*result = reinterpret_cast<T*>(vulkan.getProcAddr(name))) != nullptr;
    }

    template<typename T>
    void load_function(const vk::Instance& vulkan, const char* name, T** result) {
        if (!try_find_function(vulkan, name, result)) {
            std::stringstream err;
            err << "Unable to find vulkan function '";
            err << name;
            err << "'.";
            throw std::runtime_error(err.str());
        }
    }

}
#define VK_CALL(x) un::assertVkCall(x, #x);

#endif
