//
// Created by brunorbsf on 06/09/2020.
//

#ifndef UNNECESSARYENGINE_GRAPHICS_PIPELINE_H
#define UNNECESSARYENGINE_GRAPHICS_PIPELINE_H

#include <vulkan/vulkan.hpp>
#include <unnecessary/graphics/shader_stage.h>
#include <unnecessary/graphics/shader.h>
#include <unnecessary/graphics/vertex_layout.h>
#include <unnecessary/graphics/renderer.h>
#include <optional>

namespace un {

    /**
     * Contains all data required to build a vulkan graphics pipeline
     */
    class GraphicsPipelineBuilder {
    private:
        BoundVertexLayout vertexLayout;
        std::vector<const un::ShaderStage *> stages;
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
        explicit GraphicsPipelineBuilder(BoundVertexLayout layout);

        GraphicsPipelineBuilder(const BoundVertexLayout &layout, std::initializer_list<const ShaderStage> shaders);

        void withStandardRasterization();

        void withInputAssembly(vk::PrimitiveTopology topology);

        void addStage(const ShaderStage &stage);

        un::Pipeline build(un::Renderer &renderer, vk::RenderPass renderPass);
    };

}
#endif //UNNECESSARYENGINE_GRAPHICS_PIPELINE_H
