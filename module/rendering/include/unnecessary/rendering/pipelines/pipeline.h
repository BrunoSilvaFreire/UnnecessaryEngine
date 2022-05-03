//
// Created by brunorbsf on 17/12/2021.
//

#ifndef UNNECESSARYENGINE_PIPELINE_H
#define UNNECESSARYENGINE_PIPELINE_H

#include <optional>
#include <vulkan/vulkan.hpp>
#include <unnecessary/rendering/layout/vertex_layout.h>
#include <unnecessary/rendering/pipelines/pipeline_stage.h>
#include <unnecessary/rendering/pipelines/inputs.h>
#include <unnecessary/rendering/layout/descriptor_set_layout.h>
#include <unnecessary/rendering/window.h>

namespace un {
    class Pipeline {
    private:
        vk::Pipeline pipeline;
        vk::PipelineCache cache;
        vk::PipelineLayout layout;
    public:
        Pipeline(const vk::Pipeline& pipeline, const vk::PipelineCache& cache, const vk::PipelineLayout& layout);

        vk::Pipeline operator*() const;

        const vk::PipelineCache& getCache() const;

        const vk::PipelineLayout& getLayout() const;
    };

    class PipelineBuilder {
    private:
        std::optional<un::VertexLayout> vertexLayout;
        un::PipelineLayout pipelineLayout;
        std::vector<un::PipelineStage> stages;
        std::optional<vk::PipelineInputAssemblyStateCreateInfo> inputAssembly;
    public:

        void withInputAssembly(const vk::PipelineInputAssemblyStateCreateInfo& inputAssembly);

        void withTriangleListTopology();

        void addStage(un::PipelineStage&& stage);

        void setVertexLayout(const std::optional<un::VertexLayout>& layout);

        void setPipelineLayout(const PipelineLayout& layout);

        un::Pipeline create(vk::Device device, vk::RenderPass renderPass);
    };
}


#endif
