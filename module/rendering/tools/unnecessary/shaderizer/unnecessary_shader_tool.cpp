#include <cxxopts.hpp>
#include <iostream>
#include <unnecessary/strings.h>
#include <unnecessary/rendering/vulkan/vulkan_utils.h>
#include <unnecessary/shaderizer/shader_meta.h>
#include <unnecessary/shaderizer/shader_parsing.h>
#include <unnecessary/shaderizer/inputs/glsl_type.h>
#include <fstream>

static const char* const kShaderFileName = "shader_json";
static const char* const kOutput = "output";
static const char* const kCpp = "cpp";
static const char* const kGlsl = "glsl";

std::filesystem::path getGeneratedGlslPath(
    const std::string& output,
    const std::string& shaderName,
    const un::ShaderStageMeta& stage
);

std::filesystem::path getGeneratedCppPath(
    const std::string& output,
    const std::string& shaderName
);

void generateStageGLSL(
    const std::string& outputDirectory,
    const std::string& shaderName,
    un::ShaderMeta& meta,
    const un::ShaderStageMeta& stage
);

void generateCPP(
    const std::string& outputDirectory,
    const std::string& shaderName,
    const un::ShaderMeta& meta
);

int main(int argc, char** argv) {
    cxxopts::Options options(
        "unnecessary_shader_tool",
        "Shader preprocessor for unnecessary engine"
    );
    bool cpp, glsl;
    options.add_options()
               (kShaderFileName, "Shader json file", cxxopts::value<std::string>())
               (kOutput, "Output directory", cxxopts::value<std::string>())
               (kCpp, "Generate CPP", cxxopts::value(cpp))
               (kGlsl, "Generate kGlsl", cxxopts::value(glsl));
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

        if (glsl) {
            for (const un::ShaderStageMeta& stage : meta.getStages()) {
                generateStageGLSL(outputDirectory, shaderName, meta, stage);
            }
        }
        if (cpp) {
            generateCPP(outputDirectory, shaderName, meta);
        }
    } else {
        options.help();
    }
}

void generateCPP(
    const std::string& outputDirectory,
    const std::string& shaderName,
    const un::ShaderMeta& meta
) {
    std::string capitalizedName = un::capitalize(shaderName);
    un::GlslTypeDatabase db;
    std::stringstream cppSrc;
    std::filesystem::path reflectionPath = std::filesystem::path(outputDirectory) / "";
    std::filesystem::path generatedCppPath = getGeneratedCppPath(
        outputDirectory,
        shaderName
    );
    cppSrc << "#include <unnecessary/rendering/layout/vertex_layout.h>" << std::endl;
    cppSrc << "#include <unnecessary/rendering/layout/descriptor_set_layout.h>"
           << std::endl;
    cppSrc << "namespace un {" << std::endl;
    cppSrc << "namespace pipelines {" << std::endl;
    cppSrc << "struct " << capitalizedName << "PipelineInputs {" << std::endl;
    for (const auto& item : meta.getInputs()) {
        cppSrc << "// Scope: " << un::to_string(item.first) << std::endl;
        for (const auto& input : item.second.getInputs()) {

            auto cpp = db.glslToCpp(input.getType());
            cppSrc << "// " << input.getName() << ": "
                   << input.getType() << "(" << cpp << ")" << std::endl;

        }

    }
    cppSrc << "};" << std::endl;
    cppSrc << "inline un::VertexLayout " << "create" << capitalizedName
           << "VertexLayout() {" << std::endl;
    cppSrc << "un::VertexLayout layout;" << std::endl;
    for (const auto& item : meta.getVertexStreamLayout().getElements()) {
        cppSrc << "layout += un::VertexAttribute(\""
               << item.getName() << "\", "
               << std::to_string(item.getCount()) << ", "
               << std::to_string(item.getSingleSize()) << ", "
               << "vk::Format::e" << vk::to_string(item.getFormat())
               << ");"
               << std::endl;
    }
    cppSrc << "return layout;" << std::endl;
    cppSrc << "}" << std::endl;

    for (const auto&[scope, inputPack] : meta.getInputs()) {
        std::string stageFlags;
        for (const auto& item : meta.getStages()) {
            if (!item.isUsingInputPack(inputPack)) {
                continue;
            }
            vk::ShaderStageFlagBits bits = un::kNameToShaderStageFlags[item.getName()];
            if (!stageFlags.empty()) {
                stageFlags += " | ";
            }
            stageFlags += "vk::ShaderStageFlagBits::e";
            stageFlags += vk::to_string(bits);
        }
        if (stageFlags.empty()) {
            continue;
        }
        cppSrc << "inline un::DescriptorSetLayout create"
               << capitalizedName
               << un::capitalize(un::to_string(scope))
               << "DescriptorSetLayout() {" << std::endl;
        cppSrc << "un::DescriptorSetLayout layout(" << stageFlags << ");"
               << std::endl;
        for (const auto& item : inputPack.getInputs()) {
            cppSrc << "layout += un::Descriptor(\""
                   << item.getName() << "\", "
                   << "vk::DescriptorType::eUniformBuffer"
                   << ");"
                   << std::endl;
        }
        cppSrc << "return layout;" << std::endl;
        cppSrc << "}" << std::endl;
    }
    cppSrc << "}" << std::endl;


    cppSrc << "}" << std::endl;
    std::ofstream outputGlsl(generatedCppPath.string());
    outputGlsl << cppSrc.rdbuf();
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
        if (item.getModifier() != "in") {
            continue;
        }
        un::VertexAttributeMeta attributeMeta = meta.getVertexAttributeMeta(item.getName());

        const auto attribute = attributeMeta.getAttribute();
        src << "layout(location = " << attributeMeta.getIndex() << ") "
            << item.getModifier() << " "
            << attributeMeta.getType() << " vertex_"
            << attribute.getName() << ";" << std::endl;
    }
    src << "// Descriptor Inputs" << std::endl;
    for (const auto& item : stage.getUsedInputs()) {
        un::ShaderInputMeta inputMeta = meta.getShaderInputMeta(item);
        const std::string& inputName = inputMeta.input->getName();
        src << "layout(set=" << std::to_string(inputMeta.scope) << ", binding="
            << std::to_string(inputMeta.index) << ") uniform "
            << inputName << "_s {"
            << inputMeta.input->getType() << " " << item << ";} u_" << inputName << ";";
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

std::filesystem::path getGeneratedCppPath(
    const std::string& output,
    const std::string& shaderName
) {
    std::string filename = shaderName + ".gen.h";
    return std::filesystem::path(output) / filename;
}
