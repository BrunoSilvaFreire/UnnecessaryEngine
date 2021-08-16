#include <unnecessary/graphics/pipeline/graphics_pipeline.h>
#include <unnecessary/graphics/lighting.h>
#include <unnecessary/application.h>

namespace un {
    void GraphicsPipelineBuilder::withInputAssembly(vk::PrimitiveTopology topology) {
        inputAssembly.emplace(
            (vk::PipelineInputAssemblyStateCreateFlags) 0,
            topology
        );
    }

    void GraphicsPipelineBuilder::withStandardRasterization() {
        withInputAssembly(vk::PrimitiveTopology::eTriangleList);
        std::vector<vk::VertexInputBindingDescription> inputBindings;
        std::vector<vk::VertexInputAttributeDescription> inputAttributes;
        u32 stride = vertexLayout.getStride();
        const std::vector<un::VertexInput>& elements = vertexLayout.getElements();
        u32 count = elements.size();
        u32 offset = 0;
        u32 binding = vertexLayout.getBinding();
        for (u32 i = 0; i < count; ++i) {
            const un::VertexInput& input = elements[i];
            inputBindings.emplace_back(binding, stride);
            inputAttributes.emplace_back(
                i,
                binding,
                input.getFormat(),
                offset
            );
            offset += input.getElementSize();
        }
        vertexInput.emplace(
            (vk::PipelineVertexInputStateCreateFlags) 0,
            inputBindings,
            inputAttributes
        );
        rasterization.emplace(
            (vk::PipelineRasterizationStateCreateFlags) 0,
            true,
            true,
            vk::PolygonMode::eFill,
            (vk::CullModeFlags) vk::CullModeFlagBits::eFront,
            vk::FrontFace::eClockwise,
            false,
            0.0,
            0.0,
            0.0,
            1.0
        );
    }

    template<typename T>
    T* getOrNull(std::optional<T>& optional) {
        if (optional) {
            return optional.operator->();
        }
        return nullptr;
    }


    un::Pipeline GraphicsPipelineBuilder::build(
        un::Renderer& renderer,
        vk::RenderPass renderPass
    ) {
        vk::Device device = renderer.getVirtualDevice();
        vk::PipelineCache cache = device.createPipelineCache(
            vk::PipelineCacheCreateInfo(
                (vk::PipelineCacheCreateFlags) 0
            )
        );

        std::vector<vk::DescriptorSetLayout> layouts;
        const auto& descriptors = pipelineLayout.getDescriptorLayouts();
        for (u32 i = 0; i < descriptors.size(); ++i) {
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
                    value.size
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
        std::vector<vk::VertexInputBindingDescription> vertexBindings;
        std::vector<vk::VertexInputAttributeDescription> vertexAttributes;
        const std::vector<un::VertexInput>& vertexElements = vertexLayout.getElements();

        vertexBindings.emplace_back(
            0,
            vertexLayout.getStride()
        );
        u32 offset = 0;
        u32 binding = vertexLayout.getBinding();
        for (int i = 0; i < vertexElements.size(); ++i) {
            const un::VertexInput& input = vertexElements[i];
            vertexAttributes.emplace_back(
                i,
                binding,
                input.getFormat(),
                offset
            );
            offset += input.getElementSize();
        }
        vk::PipelineVertexInputStateCreateInfo vertexInput(
            (vk::PipelineVertexInputStateCreateFlags) 0,
            vertexBindings,
            vertexAttributes
        );


        vk::PipelineInputAssemblyStateCreateInfo inputAssembly(
            (vk::PipelineInputAssemblyStateCreateFlags) 0,
            vk::PrimitiveTopology::eTriangleList
        );
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
        vk::PipelineRasterizationStateCreateInfo rasterization(
            (vk::PipelineRasterizationStateCreateFlags) 0,
            0,
            0,
            vk::PolygonMode::eFill,
            (vk::CullModeFlags) vk::CullModeFlagBits::eBack
        );
        rasterization.setLineWidth(1.0F);
        std::vector<vk::PipelineColorBlendAttachmentState> attachments(
            {
                vk::PipelineColorBlendAttachmentState(
                    VK_TRUE,
                    vk::BlendFactor::eSrcAlpha,
                    vk::BlendFactor::eOneMinusSrcAlpha,
                    vk::BlendOp::eAdd,
                    vk::BlendFactor::eOne,
                    vk::BlendFactor::eZero,
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


        vk::GraphicsPipelineCreateInfo createInfo(
            (vk::PipelineCreateFlags) 0,
            stageInfos,
            &vertexInput,
            &inputAssembly,
            nullptr,
            &viewportState,
            &rasterization,
            &multisample,
            nullptr,
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
            str << "Unable to create graphics pipeline: '"
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
    }

    void GraphicsPipelineBuilder::addStage(const ShaderStage* stage) {
        stages.push_back(stage);
        for (const auto& item : stage->getUsedResources().getDescriptors()) {
            descriptorAccessFlags[item.set] |= stage->getFlags();
        }
    }

    GraphicsPipelineBuilder::GraphicsPipelineBuilder(BoundVertexLayout layout) : vertexLayout(
        std::move(layout)) {

    }

    GraphicsPipelineBuilder::GraphicsPipelineBuilder(
        const BoundVertexLayout& layout,
        std::initializer_list<const ShaderStage*> shaders
    ) : GraphicsPipelineBuilder(layout) {
        for (const ShaderStage* stage : shaders) {
            addStage(stage);
        }
    }

    void GraphicsPipelineBuilder::addDescriptorResource(DescriptorSetLayout&& layout) {
        pipelineLayout.push(un::DescriptorResource(std::move(layout)));
    }

    void GraphicsPipelineBuilder::addDescriptorResource(DescriptorResource&& resource) {
        pipelineLayout.push(std::move(resource));
    }

    void GraphicsPipelineBuilder::addDescriptorResource(vk::DescriptorSetLayout layout) {
        addDescriptorResource(un::DescriptorResource(layout));
    }
}