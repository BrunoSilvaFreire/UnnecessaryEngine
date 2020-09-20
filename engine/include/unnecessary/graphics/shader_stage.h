#ifndef UNNECESSARYENGINE_SHADER_STAGE_H
#define UNNECESSARYENGINE_SHADER_STAGE_H

#include <string>
#include <utility>
#include <filesystem>
#include <fstream>
#include <vulkan/vulkan.hpp>
#include <unnecessary/def.h>
#include <unnecessary/logging.h>

namespace un {
    struct DescriptorElement {
    public:
        std::string name;
        vk::DescriptorType type;
    };

    class ShaderStage {
    private:
        std::string name;
        vk::ShaderModule module;
        vk::ShaderStageFlagBits bits;
        std::vector<un::DescriptorElement> descriptorElements;
    public:
        explicit ShaderStage() = default;

        explicit ShaderStage(std::string name);

        explicit ShaderStage(const std::string &name, vk::Device &device);

        void dispose(vk::Device &device);

        void load(
                vk::Device &device,
                const std::filesystem::path &root = std::filesystem::current_path()
        );

        const std::string &getName() const;

        const vk::ShaderModule &getModule() const;

        vk::ShaderStageFlagBits getBits() const;

        const std::vector<un::DescriptorElement> &getDescriptorElements() const;
    };
}
#endif
