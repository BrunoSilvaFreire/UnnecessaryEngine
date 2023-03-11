#include <unnecessary/rendering/pipelines/pipeline.h>

namespace un {
    vk::Pipeline pipeline::operator*() const {
        return _pipeline;
    }

    const vk::PipelineCache& pipeline::get_cache() const {
        return _cache;
    }

    const vk::PipelineLayout& pipeline::get_layout() const {
        return _layout;
    }

    pipeline::pipeline(
        const vk::Pipeline& pipeline,
        const vk::PipelineCache& cache,
        const vk::PipelineLayout& layout
    ) : _pipeline(pipeline), _cache(cache), _layout(layout) {
    }

    void pipeline_builder::set_pipeline_layout(const pipeline_layout& layout) {
        _pipelineLayout = layout;
    }

    void pipeline_builder::set_vertex_layout(const std::optional<vertex_layout>& layout) {
        _vertexLayout = layout;
        with_triangle_list_topology();
    }

    void pipeline_builder::add_stage(pipeline_stage&& stage) {
        _stages.push_back(std::move(stage));
    }

    void pipeline_builder::with_triangle_list_topology() {
        with_input_assembly(
            vk::PipelineInputAssemblyStateCreateInfo(
                static_cast<vk::PipelineInputAssemblyStateCreateFlags>(0),
                vk::PrimitiveTopology::eTriangleList
            )
        );
    }

    void
    pipeline_builder::with_input_assembly(const vk::PipelineInputAssemblyStateCreateInfo& inputAssembly) {
        _inputAssembly = inputAssembly;
    }

    pipeline pipeline_builder::create(vk::Device device, vk::RenderPass renderPass) {
        vk::GraphicsPipelineCreateInfo graphicsCreateInfo;
        vk::PipelineCache cache = device.createPipelineCache(
            vk::PipelineCacheCreateInfo(
                static_cast<vk::PipelineCacheCreateFlags>(0)
            )
        );
        std::optional<vk::PipelineVertexInputStateCreateInfo> vertexInput;
        std::vector<vk::VertexInputBindingDescription> inputBindings;
        std::vector<vk::VertexInputAttributeDescription> inputAttributes;
        if (_vertexLayout) {
            if (_vertexLayout->get_length() > 0) {
                u32 stride = static_cast<u32>(_vertexLayout->get_stride());
                const std::vector<vertex_attribute>& elements = _vertexLayout->get_elements();
                u32 count = static_cast<u32>(elements.size());
                std::size_t offset = 0;
                u32 binding = 0; //TODO: Allow for multiple vertex streams
                inputBindings.emplace_back(binding, stride);
                for (u32 i = 0; i < count; ++i) {
                    const vertex_attribute& input = elements[i];
                    inputAttributes.emplace_back(
                        i,
                        binding,
                        input.get_format(),
                        static_cast<u32>(offset)
                    );
                    offset += input.get_size();
                }
            }
            vertexInput = vk::PipelineVertexInputStateCreateInfo(
                static_cast<vk::PipelineVertexInputStateCreateFlags>(0),
                inputBindings,
                inputAttributes
            );
            graphicsCreateInfo.setPVertexInputState(vertexInput.operator->());
        }
        if (_inputAssembly.has_value()) {
            graphicsCreateInfo.setPInputAssemblyState(_inputAssembly.operator->());
        }

        std::vector<vk::DescriptorSetLayout> layouts;
        for (auto& [scope, descriptors] : _pipelineLayout.get_descriptors_layout()) {
            layouts.emplace_back(descriptors.build(device));
        }
        /*const auto& descriptors = stages;
             for (u32 i = 0; i < pipelineLayout.size(); ++i) {
                 const auto& descriptor = descriptors[i];
                 switch (descriptor.getType()) {
                     case un::DescriptorSetType::eShared:
                         layouts.push_back(descriptor.getSharedSetLayout());
                         break;
                     case un::DescriptorSetType::eUnique:
                         const auto& layout = descriptor.getUniqueLayout();
                         if (layout.isEmpty()) {
                             continue;
                         }
                         layouts.push_back(layout.build(device, descriptorAccessFlags[i]));
                         break;
                 }
             }*/

        std::vector<vk::PushConstantRange> pushes;
        /* for (const un::ShaderStage* stage : stages) {
             auto& pushConstant = stage->getPushConstantRange();
             if (pushConstant.has_value()) {
                 auto value = pushConstant.value();
                 pushes.emplace_back(
                     stage->getFlags(),
                     value.offset,
                     value.singleSize
                 );
             }
         }*/
        // TODO: Add descriptor sets and push constants
        vk::PipelineLayout vkPipelineLayout = device.createPipelineLayout(
            vk::PipelineLayoutCreateInfo(
                static_cast<vk::PipelineLayoutCreateFlags>(0),
                layouts,
                pushes
            )
        );
        graphicsCreateInfo.setLayout(vkPipelineLayout);
        std::vector<vk::PipelineShaderStageCreateInfo> stageInfos;
        if (!_stages.empty()) {
            for (const pipeline_stage& stage : _stages) {
                stageInfos.emplace_back(
                    static_cast<vk::PipelineShaderStageCreateFlags>(0),
                    stage.get_stage_flags(),
                    stage.get_module(),
                    "main"
                );
            }
            graphicsCreateInfo.setStages(stageInfos);
        }

        std::array<const vk::DynamicState, 3> dynamicStates(
            {
                vk::DynamicState::eViewport,
                vk::DynamicState::eScissor,
                vk::DynamicState::eLineWidth
            }
        );
        size2d swapChainSize(100, 100);
        std::array<const vk::Viewport, 1> viewports(
            {
                vk::Viewport(
                    0, 0,
                    swapChainSize.x, swapChainSize.y,
                    0, 1
                )
            }
        );
        std::array<const vk::Rect2D, 1> scissors(
            {
                vk::Rect2D(
                    vk::Offset2D(0, 0),
                    vk::Extent2D(swapChainSize.x, swapChainSize.y)

                )
            }
        );
        vk::PipelineViewportStateCreateInfo viewportState(
            static_cast<vk::PipelineViewportStateCreateFlags>(0),
            viewports,
            scissors
        );
        graphicsCreateInfo.setPViewportState(&viewportState);
        vk::PipelineDynamicStateCreateInfo dynamicState(
            static_cast<vk::PipelineDynamicStateCreateFlags>(0),
            dynamicStates
        );
        graphicsCreateInfo.setPDynamicState(&dynamicState);
        vk::PipelineMultisampleStateCreateInfo multisample(
            static_cast<vk::PipelineMultisampleStateCreateFlags>(0),
            vk::SampleCountFlagBits::e1,
            false
        );
        graphicsCreateInfo.setPMultisampleState(&multisample);
        std::vector<vk::PipelineColorBlendAttachmentState> attachmentBlend(
            {
                vk::PipelineColorBlendAttachmentState(
                    VK_FALSE,
                    vk::BlendFactor::eSrcAlpha,
                    vk::BlendFactor::eOneMinusSrcAlpha,
                    vk::BlendOp::eAdd,
                    vk::BlendFactor::eSrcAlpha,
                    vk::BlendFactor::eOneMinusSrcAlpha,
                    vk::BlendOp::eAdd,
                    vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
                    vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA
                )
            }
        );

        vk::PipelineColorBlendStateCreateInfo colorBlend(
            static_cast<vk::PipelineColorBlendStateCreateFlags>(0),
            false,
            vk::LogicOp::eCopy,
            attachmentBlend,
            {
                0, 0, 0, 0
            }
        );
        graphicsCreateInfo.setPColorBlendState(&colorBlend);

        vk::PipelineDepthStencilStateCreateInfo depth(
            static_cast<vk::PipelineDepthStencilStateCreateFlags>(0),
            false,
            true,
            vk::CompareOp::eLess,
            false,
            false
        );
        graphicsCreateInfo.setPDepthStencilState(&depth);
        vk::PipelineRasterizationStateCreateInfo rasterization(
            static_cast<vk::PipelineRasterizationStateCreateFlags>(0),
            0,
            0,
            vk::PolygonMode::eFill,
            vk::CullModeFlagBits::eNone
        );
        graphicsCreateInfo.setPRasterizationState(&rasterization);

        //            vk::GraphicsPipelineCreateInfo createInfo(
        //                (vk::PipelineCreateFlags) 0,
        //                stageInfos,
        //                &vertexInput,
        //                inputAssembly.operator->(),
        //                nullptr,
        //                &viewportState,
        //                &rasterization,
        //                &multisample,
        //                &depth,
        //                &colorBlend,
        //                &dynamicState,
        //                vkPipelineLayout,
        //                renderPass,
        //                0 //TODO: Deferred _rendering
        //            );
        graphicsCreateInfo.setRenderPass(renderPass);
        auto pipelineResult = device.createGraphicsPipeline(
            cache,
            graphicsCreateInfo
        );
        if (pipelineResult.result != vk::Result::eSuccess) {
            std::ostringstream str;
            str
                << "Unable to create graphics pipeline: '"
                << vk::to_string(pipelineResult.result)
                << "' (" << pipelineResult.result << ").";
            throw std::runtime_error(str.str());
        }
        auto pipeline = pipelineResult.value;
        return un::pipeline(
            pipeline,
            cache,
            vkPipelineLayout
        );
    }
}
