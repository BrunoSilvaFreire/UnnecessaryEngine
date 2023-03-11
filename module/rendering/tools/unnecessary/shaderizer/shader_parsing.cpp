#include <unnecessary/rendering/pipelines/inputs.h>
#include <unnecessary/shaderizer/shader_parsing.h>
#include "unnecessary/shaderizer/inputs/glsl_type.h"
#include "shader_stage_meta.h"

namespace un {
    std::pair<rich_input, input_scope> shader_tool::parse_input(
        const std::string& name,
        const nlohmann::json& jsonInput
    ) {
        input_scope scope = global;
        if (jsonInput.contains("scope")) {
            auto scopeName = jsonInput["scope"].get<std::string>();
            if (inputs::kStringIdsToInputScope.contains(scopeName)) {
                scope = inputs::kStringIdsToInputScope.at(scopeName);
            }
        }
        return std::make_pair(
            rich_input(
                name,
                jsonInput["type"].get<std::string>()
            ),
            scope
        );
    }

    shader_meta shader_tool::parse(const std::string& shaderName, const nlohmann::json& json) {
        shader_meta meta(shaderName);

        if (json.contains(kInputsKey)) {
            auto inputs = json[kInputsKey].get<nlohmann::json>();
            for (const auto& item : inputs.items()) {
                std::string name = item.key();
                const nlohmann::json& input = item._value().get<nlohmann::json>();
                const auto [parsed, scope] = parse_input(
                    name,
                    input
                );
                meta.add_input(
                    scope,
                    std::move(const_cast<rich_input&&>(parsed))
                );
            }
        }

        if (json.contains(kStagesKey)) {
            auto shaderStages = json[kStagesKey].get<nlohmann::json>();
            for (const auto& item : shaderStages.items()) {
                std::string name = item.key();
                const nlohmann::json& input = item._value().get<nlohmann::json>();
                const shader_stage_meta& stageMeta = parse_stage(
                    name,
                    input
                );
                meta.add_stage(stageMeta);
            }
        }
        glsl_type_database database;
        if (json.contains(kVertexStreamKey)) {
            const auto& vertexStream = json[kVertexStreamKey].get<nlohmann::json>();
            vertex_layout layout;
            std::vector<std::string> types;
            for (const auto& item : vertexStream.items()) {
                std::string name = item.key();
                const nlohmann::json& input = item._value().get<nlohmann::json>();
                const auto& type = input[kTypeKey].get<std::string>();
                glsl_type* pType;
                if (database.try_find(type, &pType)) {
                    types.emplace_back(type);
                    layout += vertex_attribute(
                        name,
                        pType->get_num_elements(),
                        static_cast<u8>(pType->get_single_size()),
                        pType->get_format()
                    );
                }
            }
            meta.set_vertex_stream_layout(layout, types);
        }
        return meta;
    }

    shader_stage_meta
    shader_tool::parse_stage(const std::string& name, const nlohmann::json& jsonStage) {
        shader_stage_meta meta(name);
        if (jsonStage.contains(kInputsKey)) {
            auto inputs = jsonStage[kInputsKey].get<std::vector<std::string>>();
            for (const auto& item : inputs) {
                meta.uses_input(item);
            }
        }
        if (jsonStage.contains(kVertexStreamKey)) {
            const auto& vertexStream = jsonStage[kVertexStreamKey].get<nlohmann::json>();
            for (const auto& item : vertexStream.items()) {
                meta.uses_vertex_attribute(item.key(), item._value().get<std::string>());
            }
        }
        return meta;
    }
}
