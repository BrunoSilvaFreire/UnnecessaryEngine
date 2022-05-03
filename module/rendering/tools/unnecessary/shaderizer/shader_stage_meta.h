//
// Created by bruno on 02/05/2022.
//

#ifndef UNNECESSARYENGINE_SHADER_STAGE_META_H
#define UNNECESSARYENGINE_SHADER_STAGE_META_H


#include <unordered_map>
#include <vector>
#include <string>
#include <set>
#include <unnecessary/rendering/pipelines/inputs.h>
#include <unnecessary/rendering/layout/vertex_layout.h>
#include <unnecessary/shaderizer/inputs/shader_input.h>

namespace un {
    struct DepthInfo {
        bool enabled;
    };

    class ShaderStageMeta {
    private:
        std::string name;
        std::set<std::string> usedInputs;
        std::vector<un::InputUsage> usedVertexAttributes;
    public:
        ShaderStageMeta(const std::string& name);

        void usesInput(const std::string& input);

        void usesVertexAttribute(const std::string& vertex, const std::string& modifier);

        const std::string& getName() const;

        const std::set<std::string>& getUsedInputs() const;

        const std::vector<un::InputUsage>& getUsedVertexAttributes() const;

        bool isUsingInputPack(const un::InputPack& scope) const;
    };
}

#endif //UNNECESSARYENGINE_SHADER_STAGE_META_H
