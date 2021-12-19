#include <cxxopts.hpp>
#include <iostream>
#include <unnecessary/strings.h>
#include <unnecessary/shader_tool/shader_meta.h>
#include <unnecessary/shader_tool/shader_parsing.h>
#include <fstream>

static const char* const kShaderFileName = "shader_json";
static const char* const kOutput = "output";

std::filesystem::path getGeneratedGlslPath(
    const std::string& output,
    const std::string& shaderName,
    const un::ShaderStageMeta& stage
);

void generateStageGLSL(
    const std::string& outputDirectory,
    const std::string& shaderName,
    un::ShaderMeta& meta,
    const un::ShaderStageMeta& stage
);

int main(int argc, char** argv) {
    cxxopts::Options options(
        "unnecessary_shader_tool",
        "Shader preprocessor for unnecessary engine"
    );
    options.add_options()
               (kShaderFileName, "Shader json file", cxxopts::value<std::string>())
               (kOutput, "Output directory", cxxopts::value<std::string>());
    options.parse_positional(kShaderFileName);
    const auto& result = options.parse(argc, argv);
    size_t a = result.count(kShaderFileName);
    if (a > 0) {
        std::string shaderFile = result[kShaderFileName].as<std::string>();
        std::string outputDirectory = result[kOutput].as<std::string>();
        std::ifstream stream(shaderFile);
        nlohmann::json json;
        stream >> json;
        std::string shaderName = json["name"].get<std::string>();

        std::string capitalizedName = un::capitalize(shaderName);
        const std::string& upperName = un::upper(shaderName);
        auto meta = un::shader_tool::parse(shaderName, json);


        for (const un::ShaderStageMeta& stage : meta.getStages()) {
            generateStageGLSL(outputDirectory, shaderName, meta, stage);
        }
        std::stringstream cppSrc;

    } else {
        options.help();
    }
}

void generateStageGLSL(
    const std::string& outputDirectory,
    const std::string& shaderName,
    un::ShaderMeta& meta,
    const un::ShaderStageMeta& stage
) {
    std::stringstream src;
    std::filesystem::path generatedGlslPath = getGeneratedGlslPath(
        outputDirectory,
        shaderName,
        stage
    );
    src << "// Vertex Stream" << std::endl;
    for (const auto& item : stage.getUsedVertexAttributes()) {
        un::VertexAttributeMeta attributeMeta = meta.getVertexAttributeMeta(item);

        src << "layout(location = " << attributeMeta.getIndex() << ") vertex_"
            << attributeMeta.getAttribute().getName() << ";" << std::endl;
    }
    src << "// Descriptor Inputs" << std::endl;
    for (const auto& item : stage.getUsedInputs()) {
        un::ShaderInputMeta inputMeta = meta.getShaderInputMeta(item);
        src << "layout(set=" << std::to_string(inputMeta.scope) << ", binding="
            << std::to_string(inputMeta.index) << ") uniform "
            << inputMeta.input->getType() << " " << item << ";";
        src << "// Scope: " << un::to_string(inputMeta.scope);
        src << std::endl;
    }

    std::ofstream outputGlsl(generatedGlslPath.string());
    outputGlsl << src.rdbuf();
}

std::filesystem::path getGeneratedGlslPath(
    const std::string& output,
    const std::string& shaderName,
    const un::ShaderStageMeta& stage
) {
    std::string filename = shaderName + "." + stage.getName() + ".gen.glsl";
    return std::filesystem::path(output) / filename;
}
