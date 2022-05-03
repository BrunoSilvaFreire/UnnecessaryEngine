#include <unnecessary/shaderizer/shader_meta.h>
#include "shader_parsing.h"

namespace un {

    ShaderMeta::ShaderMeta(const std::string& name) : name(name) { }

    void ShaderMeta::addInput(un::InputScope scope, RichInput&& input) {
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

    const std::vector<ShaderStageMeta>& ShaderMeta::getStages() const {
        return stages;
    }

    const VertexLayout& ShaderMeta::getVertexStreamLayout() const {
        return vertexStreamLayout;
    }

    void ShaderMeta::setVertexStreamLayout(
        const un::VertexLayout& newLayout,
        const std::vector<std::string>& vertexStreamTypes
    ) {
        ShaderMeta::vertexStreamLayout = newLayout;
        ShaderMeta::vertexStreamTypes = vertexStreamTypes;
    }

    un::VertexAttributeMeta ShaderMeta::getVertexAttributeMeta(const std::string& vertexName) const {
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
            vertexStreamTypes[index],
            vertexStreamLayout[index]
        );
    }

    VertexAttributeMeta::VertexAttributeMeta(
        size_t index,
        std::string type,
        const VertexAttribute& attribute
    ) : index(index), attribute(attribute), type(type) { }


    size_t VertexAttributeMeta::getIndex() const {
        return index;
    }

    const VertexAttribute& VertexAttributeMeta::getAttribute() const {
        return attribute;
    }

    const std::string& VertexAttributeMeta::getType() const {
        return type;
    }

    const std::string& InputUsage::getName() const {
        return name;
    }

    const std::string& InputUsage::getModifier() const {
        return modifier;
    }

    InputUsage::InputUsage(
        const std::string& name,
        const std::string& modifier
    ) : name(name), modifier(modifier) { }
}