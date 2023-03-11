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
    class pipeline {
    private:
        vk::Pipeline _pipeline;
        vk::PipelineCache _cache;
        vk::PipelineLayout _layout;

    public:
        pipeline(
            const vk::Pipeline& pipeline,
            const vk::PipelineCache& cache,
            const vk::PipelineLayout& layout
        );

        vk::Pipeline operator*() const;

        const vk::PipelineCache& get_cache() const;

        const vk::PipelineLayout& get_layout() const;
    };

    class pipeline_builder {
    private:
        std::optional<vertex_layout> _vertexLayout;
        pipeline_layout _pipelineLayout;
        std::vector<pipeline_stage> _stages;
        std::optional<vk::PipelineInputAssemblyStateCreateInfo> _inputAssembly;

    public:
        void with_input_assembly(const vk::PipelineInputAssemblyStateCreateInfo& inputAssembly);

        void with_triangle_list_topology();

        void add_stage(pipeline_stage&& stage);

        void set_vertex_layout(const std::optional<vertex_layout>& layout);

        void set_pipeline_layout(const pipeline_layout& layout);

        pipeline create(vk::Device device, vk::RenderPass renderPass);
    };
}

#endif
