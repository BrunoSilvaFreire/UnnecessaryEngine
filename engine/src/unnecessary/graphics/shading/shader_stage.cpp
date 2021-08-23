#include <unnecessary/graphics/shading/shader_stage.h>

#include <utility>

namespace un {

    void ShaderStage::load(vk::Device& device, const std::filesystem::path& root) {
        std::filesystem::path shaderPath =
            root / "resources" / "shaders" / (name + ".spv");
        if (!std::filesystem::exists(shaderPath)) {
            std::string msg = "Unable to find shader @ ";
            msg += shaderPath;
            throw std::runtime_error(msg);
        }
        std::ifstream file(shaderPath, std::ios::ate | std::ios::binary);
        size_t size = file.tellg();
        LOG(INFO) << "Loading " << PURPLE(shaderPath.string()) << " ("
                  << GREEN(size << " bytes") << ")";
        file.seekg(0, std::ios::beg);
        std::vector<char> buf(size);
        file.read(buf.data(), size);
        loadedModule = device.createShaderModule(
            vk::ShaderModuleCreateInfo(
                (vk::ShaderModuleCreateFlags) 0,
                size,
                reinterpret_cast<u32*>(buf.data())
            )
        );
    }

    void ShaderStage::dispose(vk::Device& device) {
        device.destroy(loadedModule);
    }


    const std::string& ShaderStage::getName() const {
        return name;
    }

    const vk::ShaderModule& ShaderStage::getUnsafeModule() const {
        return loadedModule;
    }


    ShaderStage::ShaderStage(
        std::string name, const vk::ShaderStageFlagBits& flags,
        std::optional<un::PushConstants> pushConstantRange
    ) : name(std::move(name)),
        flags(flags),
        pushConstantRange(
            pushConstantRange
        ) {

    }

    ShaderStage::ShaderStage(
        const std::string& name,
        const vk::ShaderStageFlagBits& flags,
        vk::Device& device,
        std::optional<un::PushConstants> pushConstantRange,
        const std::filesystem::path& root
    )
        : ShaderStage(name, flags, pushConstantRange) {
        load(device, root);
    }

    const vk::ShaderStageFlagBits& ShaderStage::getFlags() const {
        return flags;
    }

    const std::optional<un::PushConstants>& ShaderStage::getPushConstantRange() const {
        return pushConstantRange;
    }

    const ShaderResources& ShaderStage::getUsedResources() const {
        return usedResources;
    }

    void ShaderStage::usesDescriptor(u32 set, u32 binding) {
        usedResources.descriptors.emplace_back(set, binding);
    }

    PushConstants::PushConstants(u32 offset, u32 size) : offset(offset), size(size) {}

    PushConstants::PushConstants() {}

    const std::vector<un::DescriptorReference>& ShaderResources::getDescriptors() const {
        return descriptors;
    }

}