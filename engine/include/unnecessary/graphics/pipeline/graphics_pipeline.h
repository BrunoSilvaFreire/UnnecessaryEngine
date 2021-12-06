//
// Created by brunorbsf on 06/09/2020.
//

#ifndef UNNECESSARYENGINE_GRAPHICS_PIPELINE_H
#define UNNECESSARYENGINE_GRAPHICS_PIPELINE_H

#include <vulkan/vulkan.hpp>
#include <unnecessary/graphics/shading/shader_stage.h>
#include <unnecessary/graphics/shading/shader.h>
#include <unnecessary/graphics/vertex_layout.h>
#include <unnecessary/graphics/renderer.h>
#include <unnecessary/graphics/pipeline/graphics_pipeline_layout.h>
#include <optional>

namespace un {

    /**
     * Contains all data required to build a vulkan graphics pipeline
     */
    class GraphicsPipelineBuilder {
    private:
        std::unordered_map<u32, vk::ShaderStageFlags> descriptorAccessFlags;
        BoundVertexLayout vertexLayout;
        GraphicsPipelineLayout pipelineLayout;
        std::vector<const un::ShaderStage*> stages;
        std::optional<vk::PipelineInputAssemblyStateCreateInfo> inputAssembly;
        std::optional<vk::PipelineRasterizationStateCreateInfo> rasterization;
    public:
        explicit GraphicsPipelineBuilder(BoundVertexLayout layout);

        GraphicsPipelineBuilder(
            const BoundVertexLayout& layout,
            std::initializer_list<const ShaderStage*> shaders
        );

        void withStandardRasterization();

        void withInputAssembly(vk::PrimitiveTopology topology);

        void addStage(const ShaderStage* stage);

        un::Pipeline build(un::Renderer& renderer, vk::RenderPass renderPass);

        void addDescriptorSet(un::DescriptorSetLayout&& layout);

        void addDescriptorSet(vk::DescriptorSetLayout layout);

        void addDescriptorSet(un::DescriptorSet&& resource);
    };

}
#endif //UNNECESSARYENGINE_GRAPHICS_PIPELINE_H
