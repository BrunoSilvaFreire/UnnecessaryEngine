#include <unnecessary/graphics/shader.h>

namespace un {

    Shader::Shader(
            vk::Device &device,
            std::initializer_list<const ShaderStage> shaders
    ) : moduleGraph(shaders.size()) {
        size_t index = 0;
        for (const ShaderStage &stage : shaders) {
            moduleGraph[index++] = stage;
        }
        vk::PipelineCache cache = device.createPipelineCache(
                vk::PipelineCacheCreateInfo(
                        (vk::PipelineCacheCreateFlags) 0
                )
        );
        std::vector<vk::DescriptorSetLayout> layouts;
        std::vector<vk::PushConstantRange> pushes;
        layout = device.createPipelineLayout(
                vk::PipelineLayoutCreateInfo(
                        (vk::PipelineLayoutCreateFlags) 0,
                        layouts, pushes
                )
        );
        std::vector<vk::PipelineShaderStageCreateInfo> stages;
        std::vector<vk::VertexInputBindingDescription> vertexBindings;
        std::vector<vk::VertexInputAttributeDescription> attributeBindings;
        vk::PipelineVertexInputStateCreateInfo vertexInput(
                (vk::PipelineVertexInputStateCreateFlags) 0,
                vertexBindings, attributeBindings
        );
        vk::PipelineInputAssemblyStateCreateInfo inputAssembly(
                (vk::PipelineInputAssemblyStateCreateFlags) 0,
                vk::PrimitiveTopology::ePointList
        );
        pipeline = device.createGraphicsPipeline(
                cache,
                vk::GraphicsPipelineCreateInfo(
                        (vk::PipelineCreateFlags) 0,
                        stages,
                        &vertexInput,
                        &inputAssembly
                )
        ).value;
    }


}