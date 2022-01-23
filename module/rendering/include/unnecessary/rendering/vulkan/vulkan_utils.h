//
// Created by bruno on 20/12/2021.
//

#ifndef UNNECESSARYENGINE_VULKAN_UTILS_H
#define UNNECESSARYENGINE_VULKAN_UTILS_H

#include <unordered_map>
#include <vulkan/vulkan.hpp>


namespace un {
    void assertVkCall(vk::Result result, const std::string& method);

    extern std::unordered_map<std::string, vk::ShaderStageFlagBits> kNameToShaderStageFlags;

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

}
#define vkCall(x) un::assertVkCall(x, #x);

#endif
