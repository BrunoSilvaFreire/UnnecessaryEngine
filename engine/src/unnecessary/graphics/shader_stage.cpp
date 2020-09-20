#include <unnecessary/graphics/shader_stage.h>

namespace un {

    void ShaderStage::load(vk::Device &device, const std::filesystem::path &root) {
        std::filesystem::path shaderPath = root / "resources" / "shaders" / (name + ".spv");
        std::ifstream file(shaderPath, std::ios::ate);
        size_t size = file.tellg();
        LOG(INFO) << "Loading " << PURPLE(shaderPath.string()) << " (" << GREEN(size << " bytes") << ")";
        file.seekg(0, std::ios::beg);
        std::vector<char> buf(size);
        file.read(buf.data(), size);
        module = device.createShaderModule(
                vk::ShaderModuleCreateInfo(
                        (vk::ShaderModuleCreateFlags) 0,
                        size,
                        reinterpret_cast<u32 *>(buf.data())
                )
        );
    }

    void ShaderStage::dispose(vk::Device &device) {
        device.destroy(module);
    }

    ShaderStage::ShaderStage(const std::string &name, vk::Device &device) : ShaderStage(name) {
        load(device);
    }

    ShaderStage::ShaderStage(std::string name) : name(std::move(name)) {
        DescriptorElement element;
        element.type = vk::DescriptorType::eSampler;
    }

    const std::string &ShaderStage::getName() const {
        return name;
    }

    const vk::ShaderModule &ShaderStage::getModule() const {
        return module;
    }

    vk::ShaderStageFlagBits ShaderStage::getBits() const {
        return bits;
    }

    const std::vector<un::DescriptorElement> &ShaderStage::getDescriptorElements() const {
        return descriptorElements;
    }
}