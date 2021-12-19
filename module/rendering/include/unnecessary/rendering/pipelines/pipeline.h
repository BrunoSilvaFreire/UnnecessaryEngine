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

    };

    class PipelineBuilder {
    private:
        vk::GraphicsPipelineCreateInfo pipelineInfo;
        std::optional<un::VertexLayout> vertexLayout;
    public:
        void withVertexLayout(
            const un::VertexLayout& layout
        ) {
            vk::PipelineInputAssemblyStateCreateInfo inputAssembly;
            inputAssembly.setTopology(vk::PrimitiveTopology::eTriangleList);
        }

        un::Pipeline create(vk::Device device) {


            vk::PipelineVertexInputStateCreateInfo vertexInput;
            std::vector<vk::VertexInputBindingDescription> inputBindings;
            std::vector<vk::VertexInputAttributeDescription> inputAttributes;
            if (vertexLayout) {
                u32 stride = vertexLayout->getStride();
                const std::vector<un::VertexAttribute>& elements = vertexLayout->getElements();
                u32 count = elements.size();
                u32 offset = 0;
                u32 binding = 0; // TODO: Multiple binding
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
                vertexInput = vk::PipelineVertexInputStateCreateInfo(
                    static_cast<vk::PipelineVertexInputStateCreateFlags>(0),
                    inputBindings,
                    inputAttributes
                );
            }
            device.createGraphicsPipeline(
                nullptr,
                vk::GraphicsPipelineCreateInfo(

                )
            )

        }
    };
}


#endif
