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

void addCreatePipelineLayout(const un::ShaderMeta& meta, const std::string& capitalizedName,
                             std::stringstream& cppSrc);

int main(int argc, char** argv) {
    cxxopts::Options options(
        "unnecessary_shaderizer",
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

void addCreateVertexLayoutMethod(
    const un::ShaderMeta& meta,
    const std::string& capitalizedName,
    std::stringstream& cppSrc
) {
    cppSrc << "inline un::VertexLayout create" << capitalizedName
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
    std::string upperShaderName = un::upper(shaderName);
    std::string shaderHeaderGuard = std::string("UN_SHADERIZED_") + upperShaderName + "_SOURCE_H";
    cppSrc << "#ifndef " << shaderHeaderGuard << std::endl;
    cppSrc << "#define " << shaderHeaderGuard << std::endl;

    cppSrc << "#include <unnecessary/rendering/layout/vertex_layout.h>" << std::endl;
    cppSrc << "#include <unnecessary/rendering/layout/descriptor_set_layout.h>" << std::endl;
    cppSrc << "#include <unnecessary/rendering/pipelines/pipeline.h>" << std::endl;
    cppSrc << "#define UNNECESSARY_SHADER_" << upperShaderName << "_NAME \"" << shaderName << "\"" << std::endl;
    cppSrc << "namespace un::pipelines {" << std::endl;
    cppSrc << "// Generated information: " << std::endl;
    cppSrc << std::endl;
    const un::VertexLayout& vertexLayout = meta.getVertexStreamLayout();
    const auto& vertexStreamElements = vertexLayout.getElements();
    cppSrc << "// Vertex Stream (" << vertexLayout.getLength() << " elements)" << std::endl;
    cppSrc << "// Stride: " << vertexLayout.getStride() << std::endl;
    for (std::size_t j = 0; j < vertexStreamElements.size(); ++j) {
        const auto& element = vertexStreamElements[j];
        auto attributeMeta = meta.getVertexAttributeMeta(element.getName());
        auto cpp = db.glslToCpp(attributeMeta.getType());
        cppSrc << "// -- Element #" << j << ": " << element.getName() << std::endl;
        cppSrc << "// ---- GLSL type: " << attributeMeta.getType() << std::endl;
        cppSrc << "// ---- CPP type: " << cpp << std::endl;
        cppSrc << "// ---- Attribute Index: " << attributeMeta.getIndex() << std::endl;
        cppSrc << "// ---- Size: " << static_cast<int>(element.getCount()) << " x "
               << static_cast<int>(element.getSingleSize()) << " (bytes: "
               << element.getSize() << ")" << std::endl;
        cppSrc << "// ---- Vulkan Format: " << vk::to_string(element.getFormat()) << std::endl;
        cppSrc << std::endl;
    }
    for (const auto& item : vertexStreamElements) {

    }
    cppSrc << std::endl;
    cppSrc << "// Shader Inputs (Descriptors): " << std::endl;
    for (const auto& item : meta.getInputs()) {
        un::InputScope scope = item.first;
        cppSrc << "// -- Scope: " << un::to_string(scope) << " (Uses Descriptor Set Index: " << std::to_string(scope)
               << ")" << std::endl;

        for (const auto& input : item.second.getInputs()) {
            auto cpp = db.glslToCpp(input.getType());
            un::ShaderInputMeta inputMeta = meta.getShaderInputMeta(input.getName());

            cppSrc << "// ---- Descriptor Name: " << input.getName() << std::endl;
            cppSrc << "// ------ Descriptor Index: " << inputMeta.index << std::endl;
            cppSrc << "// ------ GLSL type: " << input.getType() << std::endl;
            cppSrc << "// ------ CPP type: " << cpp << std::endl;
            cppSrc << std::endl;
        }

    }
    cppSrc << std::endl;
    cppSrc << "constexpr std::size_t " << shaderName << "ShaderVertexLayoutStride() { return "
           << vertexLayout.getStride() << ";}" << std::endl;
    addCreateVertexLayoutMethod(meta, capitalizedName, cppSrc);

    for (const auto& [scope, inputPack] : meta.getInputs()) {
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

    addCreatePipelineLayout(meta, capitalizedName, cppSrc);
    for (const auto& item : meta.getStages()) {
        const std::string& stageName = item.getName();
        cppSrc << "// Shader Stage: " << stageName << std::endl;
        std::string stageNameUpper = un::upper(stageName);
        std::string stageNameMacroPrefix =
            std::string("UNNECESSARY_SHADER_") + upperShaderName + "_STAGE_" + stageNameUpper;
        std::string shaderFileName = std::string(shaderName) + "." + stageName + ".spirv";
        cppSrc << "#define "
               << stageNameMacroPrefix << "_FILENAME \""
               << shaderFileName << "\"" << std::endl;
        cppSrc << "#define "
               << stageNameMacroPrefix << "_PATH "
               << R"("resources/shaders/)" << shaderName << "/" << shaderFileName << "\"" << std::endl;

        cppSrc << "inline un::PipelineStage create"
               << capitalizedName << "Shader"
               << un::capitalize(stageName) << "Stage(vk::Device device) {"
               << std::endl;
        cppSrc << "std::filesystem::path stagePath(" << stageNameMacroPrefix << "_PATH);" << std::endl;
        vk::ShaderStageFlagBits bits = un::kNameToShaderStageFlags[item.getName()];
        cppSrc << "return un::PipelineStage::fromFile(device, stagePath, vk::ShaderStageFlagBits::e"
               << vk::to_string(bits) << ");" << std::endl;


        cppSrc << "}" << std::endl;
    }

    cppSrc << "inline un::Pipeline create" << capitalizedName << "Pipeline(vk::Device device, vk::RenderPass pass) {"
           << std::endl;
    cppSrc << "un::PipelineBuilder builder;" << std::endl;
//    if (vertexLayout.getLength() > 0) {
        cppSrc << "builder.setVertexLayout(create" << capitalizedName << "VertexLayout());" << std::endl;
//    } else {
//        cppSrc << "builder.withTriangleListTopology();" << std::endl;
//    }
    for (const auto& item : meta.getStages()) {
        cppSrc << "builder.addStage(create" << capitalizedName << "Shader" << un::capitalize(item.getName())
               << "Stage(device));" << std::endl;
    }
    cppSrc << "return builder.create(device, pass);" << std::endl;
    cppSrc << "};" << std::endl;
    cppSrc << "}" << std::endl;
    cppSrc << "#endif" << std::endl;
    std::ofstream outputGlsl(generatedCppPath.string());
    outputGlsl << cppSrc.rdbuf();
}

void addCreatePipelineLayout(const un::ShaderMeta& meta, const std::string& capitalizedName,
                             std::stringstream& cppSrc) {
    cppSrc << "inline un::PipelineLayout create" << capitalizedName << "PipelineLayout() {" << std::endl;
    cppSrc << "return un::PipelineLayout({" << std::endl;
    for (const auto& item : meta.getInputs()) {
        cppSrc << "// Scope: " << un::to_string(item.first) << std::endl;
        std::string scopeName = un::capitalize(un::to_string(item.first));
        cppSrc << "std::make_pair(un::InputScope::e" << scopeName << ", create" << capitalizedName << scopeName
               << "DescriptorSetLayout()), " << std::endl;
    }
    cppSrc << "});" << std::endl;
    cppSrc << "};" << std::endl;
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
