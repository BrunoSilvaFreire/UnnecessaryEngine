#include <unnecessary/shaderizer/shader_meta.h>

namespace un {
    void ShaderStageMeta::usesInput(const std::string& input) {
        usedInputs.emplace(input);
    }

    ShaderStageMeta::ShaderStageMeta(const std::string& name) : name(name) { }

    const std::string& ShaderStageMeta::getName() const {
        return name;
    }

    const std::set<std::string>& ShaderStageMeta::getUsedInputs() const {
        return usedInputs;
    }

    void ShaderStageMeta::usesVertexAttribute(
        const std::string& vertex,
        const std::string& modifier
    ) {
        usedVertexAttributes.emplace_back(vertex, modifier);
    }

    const std::vector<un::InputUsage>& ShaderStageMeta::getUsedVertexAttributes() const {
        return usedVertexAttributes;
    }

    bool ShaderStageMeta::isUsingInputPack(const un::InputPack& scope) const {
        for (const auto& item : usedInputs) {
            for (const auto& other : scope.getInputs()) {
                if (other.getName() == item) {
                    return true;
                }
            }
        }
        return false;
    }
}