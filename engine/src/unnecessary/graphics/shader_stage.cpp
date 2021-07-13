#include <unnecessary/graphics/shader_stage.h>

#include <utility>

namespace un {

    void ShaderStage::load(vk::Device &device, const std::filesystem::path &root) {
        std::filesystem::path shaderPath = root / "resources" / "shaders" / (name + ".spv");
        std::ifstream file(shaderPath, std::ios::ate | std::ios::binary);
        size_t size = file.tellg();
        LOG(INFO) << "Loading " << PURPLE(shaderPath.string()) << " (" << GREEN(size << " bytes") << ")";
        file.seekg(0, std::ios::beg);
        std::vector<char> buf(size);
        file.read(buf.data(), size);
        loadedModule = device.createShaderModule(
                vk::ShaderModuleCreateInfo(
                        (vk::ShaderModuleCreateFlags) 0,
                        size,
                        reinterpret_cast<u32 *>(buf.data())
                )
        );
    }

    void ShaderStage::dispose(vk::Device &device) {
        device.destroy(loadedModule);
    }


    const std::string &ShaderStage::getName() const {
        return name;
    }

    const vk::ShaderModule &ShaderStage::getUnsafeModule() const {
        return loadedModule;
    }


    ShaderStage::ShaderStage(
            std::string name,
            const vk::ShaderStageFlagBits &flags,
            DescriptorSetLayout descriptorLayout,
            std::optional<un::PushConstants> pushConstantRange
    ) : name(std::move(name)),
        flags(flags),
        descriptorLayout(std::move(descriptorLayout)),
        pushConstantRange(pushConstantRange) {

    }

    ShaderStage::ShaderStage(
            const std::string &name,
            const vk::ShaderStageFlagBits &flags,
            const DescriptorSetLayout &descriptorLayout,
            vk::Device &device,
            std::optional<un::PushConstants> pushConstantRange,
            const std::filesystem::path &root
    ) : ShaderStage(name, flags, descriptorLayout, pushConstantRange) {
        load(device, root);
    }

    const vk::ShaderStageFlagBits &ShaderStage::getFlags() const {
        return flags;
    }

    const DescriptorSetLayout &ShaderStage::getDescriptorLayout() const {
        return descriptorLayout;
    }

    const std::optional<un::PushConstants> &ShaderStage::getPushConstantRange() const {
        return pushConstantRange;
    }

    PushConstants::PushConstants(u32 offset, u32 size) : offset(offset), size(size) {}

    PushConstants::PushConstants() {}
}