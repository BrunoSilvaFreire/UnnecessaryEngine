#ifndef UNNECESSARYENGINE_SHADER_PARSING_H
#define UNNECESSARYENGINE_SHADER_PARSING_H

#include <unnecessary/rendering/pipelines/inputs.h>
#include <unnecessary/shaderizer/shader_meta.h>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>
#include <unordered_map>
#include <unnecessary/strings.h>
#include "shader_meta.h"

namespace un::shader_tool {
    static const char* const kStagesKey = "stages";
    static const char* const kInputsKey = "inputs";
    static const char* const kVertexStreamKey = "vertex_stream";
    static const char* const kTypeKey = "type";

    std::pair<rich_input, input_scope> parse_input(
        const std::string& name,
        const nlohmann::json& jsonInput
    );

    shader_stage_meta parse_stage(
        const std::string& name,
        const nlohmann::json& jsonStage
    );

    shader_meta parse(
        const std::string& shaderName,
        const nlohmann::json& json

    );
}
#endif
