#include <unnecessary/rendering/vulkan/vulkan_utils.h>

namespace un {
    std::unordered_map<std::string, vk::ShaderStageFlagBits> kNameToShaderStageFlags = {
        std::make_pair("frag", vk::ShaderStageFlagBits::eFragment),
        std::make_pair("vert", vk::ShaderStageFlagBits::eVertex),
        std::make_pair("comp", vk::ShaderStageFlagBits::eCompute),
        std::make_pair("tess", vk::ShaderStageFlagBits::eTessellationControl),
        std::make_pair("tesse", vk::ShaderStageFlagBits::eTessellationEvaluation)
    };

    void assertVkCall(vk::Result result, const std::string& method) {
        if (result != vk::Result::eSuccess) {
            std::stringstream str;
            str << "Error while calling ";
            str << method;
            str << " (";
            str << vk::to_string(result);
            str << ")";
            throw std::runtime_error(str.str());
        }
    }
}