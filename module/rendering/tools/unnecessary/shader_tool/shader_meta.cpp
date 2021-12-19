#include <unnecessary/shader_tool/shader_meta.h>
#include <pamac.h>

namespace un {

    void InputPack::operator+=(un::Input&& other) {
        inputs.emplace_back(std::move(other));
    }

    const std::vector<un::Input>& InputPack::getInputs() const {
        return inputs;
    }

    std::vector<un::Input>& InputPack::getInputs() {
        return inputs;
    }

    ShaderMeta::ShaderMeta(const std::string& name) : name(name) {}

    void ShaderMeta::addInput(un::InputScope scope, Input&& input) {
        inputs[scope] += std::move(input);
    }

    void ShaderMeta::addStage(const ShaderStageMeta& meta) {
        stages.push_back(meta);
    }

    const std::string& ShaderMeta::getName() const {
        return name;
    }

    const std::unordered_map<un::InputScope, un::InputPack>&
    ShaderMeta::getInputs() const {
        return inputs;
    }

    const std::vector<un::ShaderStageMeta>& ShaderMeta::getStages() const {
        return stages;
    }

    const VertexLayout& ShaderMeta::getVertexStreamLayout() const {
        return vertexStreamLayout;
    }

    void ShaderMeta::setVertexStreamLayout(
        const VertexLayout& newLayout
    ) {
        ShaderMeta::vertexStreamLayout = newLayout;
    }

    un::VertexAttributeMeta
    ShaderMeta::getVertexAttributeMeta(const std::string& vertexName) {
        std::size_t index = 0;
        while (index < vertexStreamLayout.getLength()) {
            if (vertexStreamLayout[index].getName() == vertexName) {
                break;
            } else {
                index++;
            }
        }
        if (index >= vertexStreamLayout.getLength()) {
            std::string msg = "Unable to find vertex attribute with the specified name (";
            msg += vertexName;
            msg += ").";
            throw std::runtime_error(msg);
        }
        return un::VertexAttributeMeta(
            index,
            vertexStreamLayout[index]
        );
    }

    void ShaderStageMeta::usesInput(const std::string& input) {
        usedInputs.emplace(input);
    }

    ShaderStageMeta::ShaderStageMeta(const std::string& name) : name(name) {}

    const std::string& ShaderStageMeta::getName() const {
        return name;
    }

    const std::set<std::string>& ShaderStageMeta::getUsedInputs() const {
        return usedInputs;
    }

    void ShaderStageMeta::usesVertexAttribute(const std::string& vertex) {
        usedVertexAttributes.emplace(vertex);
    }

    const std::set<std::string>& ShaderStageMeta::getUsedVertexAttributes() const {
        return usedVertexAttributes;
    }

    VertexAttributeMeta::VertexAttributeMeta(
        size_t index,
        const VertexAttribute& attribute
    ) : index(index), attribute(attribute) {}


    size_t VertexAttributeMeta::getIndex() const {
        return index;
    }

    const VertexAttribute& VertexAttributeMeta::getAttribute() const {
        return attribute;
    }
}