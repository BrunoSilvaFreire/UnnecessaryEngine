#include <cxxopts.hpp>
#include <iostream>
#include <nlohmann/json.hpp>
#include <unnecessary/strings.h>
#include <fstream>
#include <filesystem>

static const char* const kShaderFileName = "shader_json";

static const char* const kShaderStage = "shader_stage";
static const char* const kOutput = "output";


std::string indent(std::size_t indentation = 1) {
    return std::string(' ', indentation);
}

int main(int argc, char** argv) {
    cxxopts::Options options("unnecessary_shader_preprocessor", "Shader preprocessor for unnecessary engine");
    options.add_options()
        (kShaderFileName, "Shader json file", cxxopts::value<std::string>())
        (kShaderStage, "Shader stage name", cxxopts::value<std::string>())
        (kOutput, "Output directory", cxxopts::value<std::string>());
    options.parse_positional(kShaderFileName);
    const auto& result = options.parse(argc, argv);
    size_t a = result.count(kShaderFileName);
    size_t b = result.count(kShaderStage);
    if (a > 0 && b > 0) {
        std::string shaderFile = result[kShaderFileName].as<std::string>();
        std::string shaderStage = result[kShaderStage].as<std::string>();
        std::string output = result[kOutput].as<std::string>();
        std::cout << "Writing generated files to directory " << output;
        std::ifstream stream(shaderFile);
        nlohmann::json json;
        stream >> json;
        std::string shaderName = json["name"].get<std::string>();

        std::string capitalizedName = un::capitalize(shaderName);

        std::stringstream src;
        src << indent();

        auto shaderStageJson = json["stages"][shaderStage].get<nlohmann::json>();
        if (shaderStageJson.contains("inputs")) {
            nlohmann::json inputs = shaderStageJson["inputs"].get<nlohmann::json>();


        }
        std::filesystem::path generatedGlslPath =
            std::filesystem::path(output) / (shaderName + "." + shaderStage + ".gen.glsl");
        std::ofstream outputGlsl(generatedGlslPath.string());
        outputGlsl << src.rdbuf();
    } else {
        options.help();
    }
}
