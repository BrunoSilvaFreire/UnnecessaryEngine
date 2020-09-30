//
// Created by brunorbsf on 06/09/2020.
//

#ifndef UNNECESSARYENGINE_PIPELINE_SPEC_H
#define UNNECESSARYENGINE_PIPELINE_SPEC_H

#include <vulkan/vulkan.hpp>
#include <unnecessary/graphics/shader_stage.h>
#include <optional>

namespace un {
    /**
     * Contains all data required to create a vulkan graphics pipeline
     */
    class GraphicsPipelineSpecification {
    private:
        std::vector<vk::PipelineShaderStageCreateInfo> stages;
        std::optional<vk::PipelineVertexInputStateCreateInfo> vertexInput;
        std::optional<vk::PipelineInputAssemblyStateCreateInfo> inputAssembly;
        std::optional<vk::PipelineTessellationStateCreateInfo> tesselation;
        std::optional<vk::PipelineViewportStateCreateInfo> viewport;
        std::optional<vk::PipelineRasterizationStateCreateInfo> rasterization;
        std::optional<vk::PipelineMultisampleStateCreateInfo> multisample;
        std::optional<vk::PipelineDepthStencilStateCreateInfo> depthStencil;
        std::optional<vk::PipelineColorBlendStateCreateInfo> colorBlend;
        std::optional<vk::PipelineDynamicStateCreateInfo> dynamicState;
    public:
        GraphicsPipelineSpecification();

        void with3DModelInputAssembly();

        void withVertexInput(
                vk::PipelineVertexInputStateCreateFlags flags,
                const std::vector<vk::VertexInputBindingDescription> &inputBindings,
                const std::vector<vk::VertexInputAttributeDescription> &inputAttributes
        ) {
            //vk::PipelineVertexInputStateCreateInfo info(flags, inputBindings, inputAttributes);
        }

        void addStage(const vk::PipelineShaderStageCreateInfo &info);

        void addStage(const ShaderStage &stage);

//        template<class... Args>
//        void addStages(Args... args) {
//            for (const ShaderStage &stage : ...args) {
//                addStage(stage);
//            }
//        }

        vk::Pipeline create(const vk::Device &device);
    };

}
#endif //UNNECESSARYENGINE_PIPELINE_SPEC_H
