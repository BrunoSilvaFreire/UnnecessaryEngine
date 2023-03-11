#include <cxxopts.hpp>
#include <iostream>
#include <unnecessary/strings.h>
#include <unnecessary/rendering/vulkan/vulkan_utils.h>
#include <unnecessary/rendering/serialization/rendering_serialization.h>
#include <unnecessary/shaderizer/shader_meta.h>
#include <unnecessary/shaderizer/shader_parsing.h>
#include <unnecessary/shaderizer/inputs/glsl_type.h>
#include <unnecessary/nlohmann/json_archiver.h>
#include <unnecessary/serialization/serializer.h>
#include <fstream>
#include "shader_stage_meta.h"

static const char* const k_shader_file_name = "shader_json";
static const char* const k_output = "output";
static const char* const k_cpp = "cpp";
static const char* const k_glsl = "glsl";

std::filesystem::path get_generated_glsl_path(
    const std::string& output,
    const std::string& shaderName,
    const un::shader_stage_meta& stage
);

std::filesystem::path get_generated_cpp_path(
    const std::string& output,
    const std::string& shaderName
);

void generate_stage_glsl(
    const std::string& outputDirectory,
    const std::string& shaderName,
    un::shader_meta& meta,
    const un::shader_stage_meta& stage
);

void generate_cpp(
    const std::string& outputDirectory,
    const std::string& shaderName,
    const un::shader_meta& meta
);

void add_create_pipeline_layout(
    const un::shader_meta& meta,
    const std::string& capitalizedName,
    std::stringstream& cppSrc
);

int main(int argc, char** argv) {
    cxxopts::Options options(
        "unnecessary_shaderizer",
        "Shader preprocessor for unnecessary engine"
    );
    bool cpp, glsl;
    options.add_options()
               (k_shader_file_name, "Shader json file", cxxopts::value<std::string>())
               (k_output, "Output directory", cxxopts::value<std::string>())
               (k_cpp, "Generate CPP", cxxopts::value(cpp))
               (k_glsl, "Generate kGlsl", cxxopts::value(glsl));
    options.parse_positional(k_shader_file_name);
    const auto& result = options.parse(argc, argv);
    size_t a = result.count(k_shader_file_name);
    if (a > 0) {
        auto shaderFile = result[k_shader_file_name].as<std::string>();
        auto outputDirectory = result[k_output].as<std::string>();
        std::ifstream stream(shaderFile);
        un::json_archiver jsonArchiver;
        un::serialized data = jsonArchiver.read_file(shaderFile);
        auto pipeline = un::serialization::deserialize_structure<un::pipeline_descrition>(data);
        nlohmann::json json;
        stream >> json;
        auto shaderName = json["name"].get<std::string>();

        std::string capitalizedName = un::capitalize(shaderName);
        const std::string& upperName = un::upper(shaderName);
        auto meta = un::shader_tool::parse(shaderName, json);

        if (glsl) {
            for (const un::shader_stage_meta& stage : meta.get_stages()) {
                generate_stage_glsl(outputDirectory, shaderName, meta, stage);
            }
        }
        if (cpp) {
            generate_cpp(outputDirectory, shaderName, meta);
        }
    }
    else {
        options.help();
    }
}

void addCreateVertexLayoutMethod(
    const un::shader_meta& meta,
    const std::string& capitalizedName,
    std::stringstream& cppSrc
) {
    cppSrc << "inline un::VertexLayout create" << capitalizedName
           << "VertexLayout() {" << std::endl;
    cppSrc << "un::VertexLayout layout;" << std::endl;
    for (const auto& item : meta.get_vertex_stream_layout().get_elements()) {
        cppSrc << "layout += un::vertex_attribute(\""
               << item.get_name() << "\", "
               << std::to_string(item.get_count()) << ", "
               << std::to_string(item.get_single_size()) << ", "
               << "vk::Format::e" << to_string(item.get_format())
               << ");"
               << std::endl;
    }
    cppSrc << "return layout;" << std::endl;
    cppSrc << "}" << std::endl;
}

void generate_cpp(
    const std::string& outputDirectory,
    const std::string& shaderName,
    const un::shader_meta& meta
) {
    std::string capitalizedName = un::capitalize(shaderName);
    un::glsl_type_database db;
    std::stringstream cppSrc;
    std::filesystem::path reflectionPath = std::filesystem::path(outputDirectory) / "";
    std::filesystem::path generatedCppPath = get_generated_cpp_path(
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
    cppSrc << "#define UNNECESSARY_SHADER_" << upperShaderName << "_NAME \"" << shaderName << "\""
           << std::endl;
    cppSrc << "namespace un::pipelines {" << std::endl;
    cppSrc << "// Generated information: " << std::endl;
    cppSrc << std::endl;
    const un::vertex_layout& vertexLayout = meta.get_vertex_stream_layout();
    const auto& vertexStreamElements = vertexLayout.get_elements();
    cppSrc << "// Vertex Stream (" << vertexLayout.get_length() << " elements)" << std::endl;
    cppSrc << "// Stride: " << vertexLayout.get_stride() << std::endl;
    for (std::size_t j = 0; j < vertexStreamElements.size(); ++j) {
        const auto& element = vertexStreamElements[j];
        auto attributeMeta = meta.get_vertex_attribute_meta(element.get_name());
        auto cpp = db.glsl_to_cpp(attributeMeta.get_type());
        cppSrc << "// -- Element #" << j << ": " << element.get_name() << std::endl;
        cppSrc << "// ---- GLSL type: " << attributeMeta.get_type() << std::endl;
        cppSrc << "// ---- CPP type: " << cpp << std::endl;
        cppSrc << "// ---- Attribute Index: " << attributeMeta.get_index() << std::endl;
        cppSrc << "// ---- Size: " << static_cast<int>(element.get_count()) << " x "
               << static_cast<int>(element.get_single_size()) << " (bytes: "
               << element.get_size() << ")" << std::endl;
        cppSrc << "// ---- Vulkan Format: " << to_string(element.get_format()) << std::endl;
        cppSrc << std::endl;
    }
    for (const auto& item : vertexStreamElements) {
    }
    cppSrc << std::endl;
    cppSrc << "// Shader Inputs (Descriptors): " << std::endl;
    for (const auto& item : meta.get_inputs()) {
        un::input_scope scope = item.first;
        cppSrc << "// -- Scope: " << to_string(scope) << " (Uses Descriptor Set Index: "
               << std::to_string(scope)
               << ")" << std::endl;

        for (const auto& input : item.second.get_inputs()) {
            auto cpp = db.glsl_to_cpp(input.get_type());
            un::shader_input_meta inputMeta = meta.get_shader_input_meta(input.get_name());

            cppSrc << "// ---- Descriptor Name: " << input.get_name() << std::endl;
            cppSrc << "// ------ Descriptor Index: " << inputMeta.index << std::endl;
            cppSrc << "// ------ GLSL type: " << input.get_type() << std::endl;
            cppSrc << "// ------ CPP type: " << cpp << std::endl;
            cppSrc << std::endl;
        }
    }
    cppSrc << std::endl;
    cppSrc << "constexpr std::size_t " << shaderName << "ShaderVertexLayoutStride() { return "
           << vertexLayout.get_stride() << ";}" << std::endl;
    addCreateVertexLayoutMethod(meta, capitalizedName, cppSrc);

    for (const auto& [scope, inputPack] : meta.get_inputs()) {
        std::string stageFlags;
        for (const auto& item : meta.get_stages()) {
            if (!item.is_using_input_pack(inputPack)) {
                continue;
            }
            vk::ShaderStageFlagBits bits = un::kNameToShaderStageFlags[item.get_name()];
            if (!stageFlags.empty()) {
                stageFlags += " | ";
            }
            stageFlags += "vk::ShaderStageFlagBits::e";
            stageFlags += to_string(bits);
        }
        if (stageFlags.empty()) {
            continue;
        }
        cppSrc << "inline un::DescriptorSetLayout create"
               << capitalizedName
               << un::capitalize(to_string(scope))
               << "DescriptorSetLayout() {" << std::endl;
        cppSrc << "un::DescriptorSetLayout layout(" << stageFlags << ");"
               << std::endl;
        for (const auto& item : inputPack.get_inputs()) {
            cppSrc << "layout += un::Descriptor(\""
                   << item.get_name() << "\", "
                   << "vk::DescriptorType::eUniformBuffer"
                   << ");"
                   << std::endl;
        }
        cppSrc << "return layout;" << std::endl;
        cppSrc << "}" << std::endl;
    }

    add_create_pipeline_layout(meta, capitalizedName, cppSrc);
    for (const auto& item : meta.get_stages()) {
        const std::string& stageName = item.get_name();
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
               << R"("resources/shaders/)" << shaderName << "/" << shaderFileName << "\""
               << std::endl;

        cppSrc << "inline un::PipelineStage create"
               << capitalizedName << "Shader"
               << un::capitalize(stageName) << "Stage(vk::Device device) {"
               << std::endl;
        cppSrc << "std::filesystem::path stagePath(" << stageNameMacroPrefix << "_PATH);"
               << std::endl;
        vk::ShaderStageFlagBits bits = un::kNameToShaderStageFlags[item.get_name()];
        cppSrc
            << "return un::PipelineStage::from_file(device, stagePath, vk::ShaderStageFlagBits::e"
            << to_string(bits) << ");" << std::endl;

        cppSrc << "}" << std::endl;
    }

    cppSrc << "inline un::Pipeline create" << capitalizedName
           << "Pipeline(vk::Device device, vk::RenderPass pass) {"
           << std::endl;
    cppSrc << "un::PipelineBuilder builder;" << std::endl;
    //    if (vertexLayout.getLength() > 0) {
    cppSrc << "builder.set_vertex_layout(create" << capitalizedName << "VertexLayout());"
           << std::endl;
    //    } else {
    //        cppSrc << "builder.withTriangleListTopology();" << std::endl;
    //    }
    for (const auto& item : meta.get_stages()) {
        cppSrc << "builder.add_stage(create" << capitalizedName << "Shader"
               << un::capitalize(item.get_name())
               << "Stage(device));" << std::endl;
    }
    cppSrc << "return builder.create(device, pass);" << std::endl;
    cppSrc << "};" << std::endl;
    cppSrc << "}" << std::endl;
    cppSrc << "#endif" << std::endl;
    std::ofstream outputGlsl(generatedCppPath.string());
    outputGlsl << cppSrc.rdbuf();
}

void add_create_pipeline_layout(
    const un::shader_meta& meta, const std::string& capitalizedName,
    std::stringstream& cppSrc
) {
    cppSrc << "inline un::PipelineLayout create" << capitalizedName << "PipelineLayout() {"
           << std::endl;
    cppSrc << "return un::PipelineLayout({" << std::endl;
    for (const auto& item : meta.get_inputs()) {
        cppSrc << "// Scope: " << to_string(item.first) << std::endl;
        std::string scopeName = un::capitalize(to_string(item.first));
        cppSrc << "std::make_pair(un::InputScope::e" << scopeName << ", create" << capitalizedName
               << scopeName
               << "DescriptorSetLayout()), " << std::endl;
    }
    cppSrc << "});" << std::endl;
    cppSrc << "};" << std::endl;
}

void generate_stage_glsl(
    const std::string& outputDirectory,
    const std::string& shaderName,
    un::shader_meta& meta,
    const un::shader_stage_meta& stage
) {
    std::stringstream src;
    std::filesystem::path generatedGlslPath = get_generated_glsl_path(
        outputDirectory,
        shaderName,
        stage
    );
    src << "// Vertex Stream" << std::endl;
    for (const auto& item : stage.get_used_vertex_attributes()) {
        if (item.get_modifier() != "in") {
            continue;
        }
        un::vertex_attribute_meta attributeMeta = meta.get_vertex_attribute_meta(item.get_name());

        const auto attribute = attributeMeta.get_attribute();
        src << "layout(location = " << attributeMeta.get_index() << ") "
            << item.get_modifier() << " "
            << attributeMeta.get_type() << " vertex_"
            << attribute.get_name() << ";" << std::endl;
    }
    src << "// Descriptor Inputs" << std::endl;
    for (const auto& item : stage.get_used_inputs()) {
        un::shader_input_meta inputMeta = meta.get_shader_input_meta(item);
        const std::string& inputName = inputMeta.input->get_name();
        src << "layout(set=" << std::to_string(inputMeta.scope) << ", binding="
            << std::to_string(inputMeta.index) << ") uniform "
            << inputName << "_s {"
            << inputMeta.input->get_type() << " " << item << ";} u_" << inputName << ";";
        src << "// Scope: " << to_string(inputMeta.scope);
        src << std::endl;
    }

    std::ofstream outputGlsl(generatedGlslPath.string());
    outputGlsl << src.rdbuf();
}

std::filesystem::path get_generated_glsl_path(
    const std::string& output,
    const std::string& shaderName,
    const un::shader_stage_meta& stage
) {
    std::string filename = shaderName + "." + stage.get_name() + ".gen.glsl";
    return std::filesystem::path(output) / filename;
}

std::filesystem::path get_generated_cpp_path(
    const std::string& output,
    const std::string& shaderName
) {
    std::string filename = shaderName + ".gen.h";
    return std::filesystem::path(output) / filename;
}
