//
// Created by brunorbsf on 17/12/2021.
//

#ifndef UNNECESSARYENGINE_PIPELINE_H
#define UNNECESSARYENGINE_PIPELINE_H

#include <optional>
#include <vulkan/vulkan.hpp>
#include <unnecessary/rendering/layout/vertex_layout.h>

namespace un {
    class Pipeline {
    private:
        vk::Pipeline pipeline;
        vk::PipelineCache cache;
        vk::PipelineLayout layout;
    public:
        vk::Pipeline operator*() const;

        const vk::PipelineCache& getCache() const;

        const vk::PipelineLayout& getLayout() const;
    };

    class PipelineBuilder {
    private:
        std::optional<un::VertexLayout> vertexLayout;
    public:
        void withVertexLayout(
            const un::VertexLayout& layout
        ) {
            vk::PipelineInputAssemblyStateCreateInfo inputAssembly;
            inputAssembly.setTopology(vk::PrimitiveTopology::eTriangleList);
        }

        un::Pipeline create(vk::Device device) {


            vk::PipelineCache cache = device.createPipelineCache(
                vk::PipelineCacheCreateInfo(
                    (vk::PipelineCacheCreateFlags) 0
                )
            );
            std::vector<vk::VertexInputBindingDescription> inputBindings;
            std::vector<vk::VertexInputAttributeDescription> inputAttributes;
            u32 stride = vertexLayout->getStride();
            const std::vector<un::VertexAttribute>& elements = vertexLayout->getElements();
            u32 count = elements.size();
            u32 offset = 0;
            u32 binding = 0; //TODO: Allow for multiple vertex streams
            inputBindings.emplace_back(binding, stride);
            for (u32 i = 0; i < count; ++i) {
                const un::VertexAttribute& input = elements[i];
                inputAttributes.emplace_back(
                    i,
                    binding,
                    input.getFormat(),
                    offset
                );
                offset += input.getSize();
            }
            vk::PipelineVertexInputStateCreateInfo vertexInput(
                (vk::PipelineVertexInputStateCreateFlags) 0,
                inputBindings,
                inputAttributes
            );
            std::vector<vk::DescriptorSetLayout> layouts;
            /*        const auto& descriptors = pipelineLayout.getDescriptorLayouts();
                    for (u32 i = 0; i < descriptors.singleSize(); ++i) {
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
                    }

                    std::vector<vk::PushConstantRange> pushes;
                    for (const un::ShaderStage* stage : stages) {
                        auto& pushConstant = stage->getPushConstantRange();
                        if (pushConstant.has_value()) {
                            auto value = pushConstant.value();
                            pushes.emplace_back(
                                stage->getFlags(),
                                value.offset,
                                value.singleSize
                            );
                        }
                    }
                    // TODO: Add descriptor sets and push constants
                    vk::PipelineLayout vkPipelineLayout = device.createPipelineLayout(
                        vk::PipelineLayoutCreateInfo(
                            (vk::PipelineLayoutCreateFlags) 0,
                            layouts, pushes
                        )
                    );
                    std::vector<vk::PipelineShaderStageCreateInfo> stageInfos;
                    for (const un::ShaderStage* stage : stages) {
                        stageInfos.emplace_back(
                            (vk::PipelineShaderStageCreateFlags) 0,
                            stage->getFlags(),
                            stage->getUnsafeModule(),
                            "main"
                        );
                    }

                    std::array<const vk::DynamicState, 2> dynamicStates(
                        {
                            vk::DynamicState::eViewport,
                            vk::DynamicState::eScissor
                        }
                    );
                    un::Size2D swapChainSize = renderer.getSwapChain().getResolution();
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
                        (vk::PipelineViewportStateCreateFlags) 0,
                        viewports,
                        scissors
                    );
                    vk::PipelineDynamicStateCreateInfo dynamicState((vk::PipelineDynamicStateCreateFlags) 0,
                                                                    dynamicStates
                    );
                    vk::PipelineMultisampleStateCreateInfo multisample(
                        (vk::PipelineMultisampleStateCreateFlags) 0,
                        vk::SampleCountFlagBits::e1,
                        false
                    );
                    std::vector<vk::PipelineColorBlendAttachmentState> attachments(
                        {
                            vk::PipelineColorBlendAttachmentState(
                                VK_TRUE,
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
                        (vk::PipelineColorBlendStateCreateFlags) 0,
                        false,
                        vk::LogicOp::eCopy,
                        attachments,
                        {
                            0, 0, 0, 0
                        }
                    );

                    vk::PipelineDepthStencilStateCreateInfo depth(
                        static_cast<vk::PipelineDepthStencilStateCreateFlags>(0),
                        true,
                        true,
                        vk::CompareOp::eLess,
                        false,
                        false
                    );
                    vk::GraphicsPipelineCreateInfo createInfo(
                        (vk::PipelineCreateFlags) 0,
                        stageInfos,
                        &vertexInput,
                        inputAssembly.operator->(),
                        nullptr,
                        &viewportState,
                        rasterization.operator->(),
                        &multisample,
                        &depth,
                        &colorBlend,
                        &dynamicState,
                        vkPipelineLayout,
                        renderPass,
                        0 //TODO: Deferred rendering
                    );

                    auto pipelineResult = device.createGraphicsPipeline(
                        cache,
                        createInfo
                    );
                    if (pipelineResult.result != vk::Result::eSuccess) {
                        std::ostringstream str;
                        str
                            << "Unable to clayout(location = 0) in vec3 position;reate graphics pipeline: '"
                            << vk::to_string(pipelineResult.result)
                            << "' (" << pipelineResult.result << ").";
                        throw std::runtime_error(str.str());
                    }
                    auto pipeline = pipelineResult.value;
                    return un::Pipeline(
                        std::string("standart"),
                        un::PipelineData{
                            cache,
                            vkPipelineLayout,
                            pipeline
                        }
                    );
        */
        }
    };
}


#endif
