#ifndef UNNECESSARYENGINE_SHADER_META_H
#define UNNECESSARYENGINE_SHADER_META_H

#include <unordered_map>
#include <vector>
#include <string>
#include <set>
#include <unnecessary/rendering/pipelines/inputs.h>
#include <unnecessary/rendering/layout/vertex_layout.h>

namespace un {

    class InputPack {
    private:
        std::vector<un::Input> inputs;
    public:
        void operator+=(un::Input&& other);

        const std::vector<un::Input>& getInputs() const;

        std::vector<un::Input>& getInputs();
    };

    class ShaderStageMeta {
    private:
        std::string name;
        std::set<std::string> usedInputs;
        std::set<std::string> usedVertexAttributes;
    public:
        ShaderStageMeta(const std::string& name);

        void usesInput(const std::string& input);

        void usesVertexAttribute(const std::string& vertex);

        const std::string& getName() const;

        const std::set<std::string>& getUsedInputs() const;

        const std::set<std::string>& getUsedVertexAttributes() const;
    };

    struct VertexAttributeMeta {
    private:
        std::size_t index;
        un::VertexAttribute attribute;
    public:
        VertexAttributeMeta(
            size_t index,
            const VertexAttribute& attribute
        );

        size_t getIndex() const;

        const VertexAttribute& getAttribute() const;
    };

    struct ShaderInputMeta {
    public:
        size_t index;
        un::InputScope scope;
        un::Input* input;

    };

    class ShaderMeta {
    private:
        std::string name;
        std::unordered_map<un::InputScope, un::InputPack> inputs;
        std::vector<un::ShaderStageMeta> stages;
        un::VertexLayout vertexStreamLayout;
    public:

        ShaderMeta(const std::string& name);

        void addInput(un::InputScope scope, un::Input&& input);

        void addStage(const un::ShaderStageMeta& meta);

        const VertexLayout& getVertexStreamLayout() const;

        void setVertexStreamLayout(const un::VertexLayout& newLayout);

        un::VertexAttributeMeta getVertexAttributeMeta(const std::string& vertexName);

        const std::string& getName() const;

        const std::unordered_map<un::InputScope, un::InputPack>& getInputs() const;

        const std::vector<un::ShaderStageMeta>& getStages() const;

        un::ShaderInputMeta getShaderInputMeta(
            std::string inputName
        ) {
            constexpr std::array<un::InputScope, 3> scopes = {
                un::InputScope::eGlobal,
                un::InputScope::ePipeline,
                un::InputScope::eInstance
            };
            for (un::InputScope scope : scopes) {
                un::InputPack& pack = inputs[scope];
                std::vector<un::Input>& packInputs = pack.getInputs();
                for (std::size_t i = 0; i < packInputs.size(); ++i) {
                    un::Input& input = packInputs[i];
                    if (input.getName() != inputName) {
                        continue;
                    }
                    un::ShaderInputMeta meta;
                    meta.index = i;
                    meta.scope = scope;
                    meta.input = &input;
                    return meta;
                }
            }
            std::string msg = "Unable to find input with the given name (";
            msg += inputName;
            msg += ").";
            throw std::runtime_error(msg);
        }
    };
}
#endif
