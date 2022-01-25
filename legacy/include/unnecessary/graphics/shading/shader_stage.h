#ifndef UNNECESSARYENGINE_SHADER_STAGE_H
#define UNNECESSARYENGINE_SHADER_STAGE_H

#include <string>
#include <utility>
#include <filesystem>
#include <fstream>
#include <optional>
#include <vulkan/vulkan.hpp>
#include <unnecessary/def.h>
#include <unnecessary/logging.h>
#include <unnecessary/graphics/descriptors/descriptor_layout.h>
#include <unnecessary/misc/membuf.h>

namespace un {
    struct PushConstants {
    public:
        u32 offset, size;

        PushConstants();

        PushConstants(u32 offset, u32 size);
    };

    class ShaderResources {
    private:
        std::vector<DescriptorReference> descriptors;


    public:
        friend class ShaderStage;

        const std::vector<DescriptorReference>& getDescriptors() const;

    };

    class ShaderStage {
    private:
        std::string name;
        /**
         * The currently loaded module in vulkan.
         * May be null if the SPIR-V has not yet been uploaded to the GPU
         */
        vk::ShaderModule loadedModule;
        vk::ShaderStageFlagBits flags;
        std::optional<un::PushConstants> pushConstantRange;
        ShaderResources usedResources;
    public:
        ShaderStage(
            std::string name,
            const vk::ShaderStageFlagBits& flags,
            std::optional<un::PushConstants> pushConstantRange = std::optional<un::PushConstants>()
        );

        ShaderStage(
            std::string name,
            const vk::ShaderStageFlagBits& flags,
            vk::Device device,
            const un::Buffer& content,
            std::optional<un::PushConstants> pushConstantRange = std::optional<un::PushConstants>());

        ShaderStage(
            const std::string& name,
            const vk::ShaderStageFlagBits& flags,
            vk::Device& device,
            std::optional<un::PushConstants> pushConstantRange = std::optional<un::PushConstants>(),
            const std::filesystem::path& root = std::filesystem::current_path()
        );

        void dispose(vk::Device& device);

        void load(
            vk::Device& device,
            const std::filesystem::path& root = std::filesystem::current_path()
        );

        void usesDescriptor(u32 set, u32 binding);

        const std::string& getName() const;

        const vk::ShaderModule& getUnsafeModule() const;

        const vk::ShaderStageFlagBits& getFlags() const;

        const std::optional<un::PushConstants>& getPushConstantRange() const;

        const ShaderResources& getUsedResources() const;
    };


}
#endif