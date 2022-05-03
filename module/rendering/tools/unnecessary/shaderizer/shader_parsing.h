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

namespace un {
    namespace shader_tool {
        static const char* const kStagesKey = "stages";
        static const char* const kInputsKey = "inputs";
        static const char* const kVertexStreamKey = "vertex_stream";
        static const char* const kTypeKey = "type";

        std::pair<RichInput, un::InputScope> parseInput(
            const std::string& name,
            const nlohmann::json& jsonInput
        );

        un::ShaderStageMeta parseStage(
            const std::string& name,
            const nlohmann::json& jsonStage
        );

        un::ShaderMeta parse(
            const std::string& shaderName,
            const nlohmann::json& json

        );
    }
}
#endif
