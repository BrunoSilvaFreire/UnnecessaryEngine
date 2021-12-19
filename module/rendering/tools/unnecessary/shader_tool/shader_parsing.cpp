#include <unnecessary/shader_tool/shader_parsing.h>
#include "unnecessary/shader_tool/inputs/glsl_type.h"

namespace un {

    std::pair<un::Input, un::InputScope> shader_tool::parseInput(
        const std::string& name,
        const nlohmann::json& jsonInput
    ) {
        un::InputScope scope = un::InputScope::eGlobal;
        if (jsonInput.contains("scope")) {
            auto scopeName = jsonInput["scope"].get<std::string>();
            if (un::inputs::kStringIdsToLessonTypes.contains(scopeName)) {
                scope = un::inputs::kStringIdsToLessonTypes.at(scopeName);
            }
        }
        return std::make_pair(
            un::Input(
                name,
                jsonInput["type"].get<std::string>()
            ),
            scope
        );
    }

    un::ShaderMeta
    shader_tool::parse(const std::string& shaderName, const nlohmann::json& json) {
        un::ShaderMeta meta(shaderName);

        if (json.contains(kInputsKey)) {
            nlohmann::json inputs = json[kInputsKey].get<nlohmann::json>();
            for (const auto& item : inputs.items()) {
                std::string name = item.key();
                const nlohmann::json& input = item.value().get<nlohmann::json>();
                const auto[parsed, scope] = un::shader_tool::parseInput(
                    name,
                    input
                );
                meta.addInput(
                    scope,
                    std::move(const_cast<un::Input&&>(parsed))
                );
            }
        }

        if (json.contains(kStagesKey)) {
            auto shaderStages = json[kStagesKey].get<nlohmann::json>();
            for (const auto& item : shaderStages.items()) {
                std::string name = item.key();
                const nlohmann::json& input = item.value().get<nlohmann::json>();
                const ShaderStageMeta& stageMeta = un::shader_tool::parseStage(
                    name,
                    input
                );
                meta.addStage(stageMeta);
            }
        }
        un::GlslTypeDatabase database;
        if (json.contains(kVertexStreamKey)) {
            const auto& vertexStream = json[kVertexStreamKey].get<nlohmann::json>();
            un::VertexLayout layout;
            for (const auto& item : vertexStream.items()) {
                std::string name = item.key();
                const nlohmann::json& input = item.value().get<nlohmann::json>();
                const auto& type = input[kTypeKey].get<std::string>();
                un::GlslType* pType;
                if (database.tryFind(type, &pType)) {
                    layout += un::VertexAttribute(
                        name,
                        pType->getNumElements(),
                        pType->getSingleSize(),
                        pType->getFormat()
                    );
                }
            }
            meta.setVertexStreamLayout(layout);
        }
        return meta;
    }

    un::ShaderStageMeta
    shader_tool::parseStage(const std::string& name, const nlohmann::json& jsonStage) {
        un::ShaderStageMeta meta(name);
        if (jsonStage.contains(kInputsKey)) {
            auto inputs = jsonStage[kInputsKey].get<std::vector<std::string>>();
            for (const auto& item : inputs) {
                meta.usesInput(item);
            }
        }
        if (jsonStage.contains(kVertexStreamKey)) {
            const auto& vertexStream = jsonStage[kVertexStreamKey].get<std::vector<std::string>>();
            for (const auto& item : vertexStream) {
                meta.usesVertexAttribute(item);
            }
        }
        return meta;
    }
}