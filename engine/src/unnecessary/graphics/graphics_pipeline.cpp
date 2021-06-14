#include <unnecessary/graphics/graphics_pipeline.h>
#include <unnecessary/graphics/vertex_layout.h>
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
        const std::vector<un::VertexInput> &elements = vertexLayout.getElements();
        u32 count = elements.size();
        u32 offset = 0;
        u32 binding = vertexLayout.getBinding();
        for (u32 i = 0; i < count; ++i) {
            const un::VertexInput &input = elements[i];
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
        std::vector<vk::PipelineColorBlendAttachmentState> states;
        states.emplace_back(
                true,
                vk::BlendFactor::eSrcAlpha,
                vk::BlendFactor::eOneMinusSrcAlpha
        );
        vk::PipelineColorBlendStateCreateInfo colorBlend(
                (vk::PipelineColorBlendStateCreateFlags) 0,
                true,
                vk::LogicOp::eClear,
                states
        );
    }

    template<typename T>
    T *getOrNull(std::optional<T> &optional) {
        if (optional) {
            return optional.operator->();
        }
        return nullptr;
    }


    un::Pipeline GraphicsPipelineBuilder::build(un::Renderer &renderer) {
        vk::Device device = renderer.getVirtualDevice();
        vk::PipelineCache cache = device.createPipelineCache(
                vk::PipelineCacheCreateInfo(
                        (vk::PipelineCacheCreateFlags) 0
                )
        );
        std::vector<vk::DescriptorSetLayout> layouts;
        std::vector<vk::PushConstantRange> pushes;
        for (const un::ShaderStage *stage : stages) {
            auto &pushConstant = stage->getPushConstantRange();
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
        vk::PipelineLayout pipelineLayout = device.createPipelineLayout(
                vk::PipelineLayoutCreateInfo(
                        (vk::PipelineLayoutCreateFlags) 0,
                        layouts, pushes
                )
        );
        std::vector<vk::PipelineShaderStageCreateInfo> stageInfos;
        for (const un::ShaderStage *stage : stages) {
            stageInfos.emplace_back(
                    (vk::PipelineShaderStageCreateFlags) 0,
                    stage->getFlags(),
                    stage->getUnsafeModule(),
                    "main"
            );
        }
        std::vector<vk::VertexInputBindingDescription> vertexBindings;
        std::vector<vk::VertexInputAttributeDescription> vertexAttributes;
        const std::vector<un::VertexInput> &vertexElements = vertexLayout.getElements();

        vertexBindings.emplace_back(
                0,
                vertexLayout.getStride()
        );
        u32 offset = 0;
        u32 binding = vertexLayout.getBinding();
        for (int i = 0; i < vertexElements.size(); ++i) {
            const un::VertexInput &input = vertexElements[i];
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
        vk::PipelineDynamicStateCreateInfo dynamicState((vk::PipelineDynamicStateCreateFlags) 0, dynamicStates);
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
        std::vector<vk::PipelineColorBlendAttachmentState> attachments = {
                vk::PipelineColorBlendAttachmentState(
                        VK_TRUE,
                        vk::BlendFactor::eSrcAlpha,
                        vk::BlendFactor::eOneMinusSrcAlpha
                )
        };
        vk::PipelineColorBlendStateCreateInfo colorBlend(
                (vk::PipelineColorBlendStateCreateFlags) 0,
                0,
                vk::LogicOp::eClear,
                attachments
        );

        vk::AttachmentDescription colorAttachment(
                (vk::AttachmentDescriptionFlags) 0,
                renderer.getSwapChain().getFormat()
        );
        colorAttachment.setFinalLayout(vk::ImageLayout::eGeneral);
        colorAttachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
        colorAttachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;

        std::vector<vk::AttachmentReference> colorAttachments;
        std::vector<vk::AttachmentReference> inputAttachments;
        std::vector<vk::SubpassDescription> subpasses(
                {
                        vk::SubpassDescription(

                                (vk::SubpassDescriptionFlags) 0,
                                vk::PipelineBindPoint::eGraphics,
                                inputAttachments,
                                colorAttachments
                        )
                }
        );

        std::array<vk::AttachmentDescription, 1> attachmentDescriptions = {
                colorAttachment
        };
        auto renderPass = device.createRenderPass(
                vk::RenderPassCreateInfo(
                        (vk::RenderPassCreateFlags) 0,
                        attachmentDescriptions,
                        subpasses
                )
        );
        auto pipeline = device.createGraphicsPipeline(
                cache,
                vk::GraphicsPipelineCreateInfo(
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
                        pipelineLayout,
                        renderPass,
                        0 //TODO: Deferred rendering
                )
        ).value;

        return un::Pipeline(
                std::string("standart"),
                un::PipelineData{
                        cache,
                        pipelineLayout,
                        pipeline
                }
        );
    }

    void GraphicsPipelineBuilder::addStage(const ShaderStage &stage) {
        stages.push_back(&stage);
    }

    GraphicsPipelineBuilder::GraphicsPipelineBuilder(BoundVertexLayout layout) : vertexLayout(std::move(layout)) {

    }

    GraphicsPipelineBuilder::GraphicsPipelineBuilder(
            const BoundVertexLayout &layout,
            std::initializer_list<const ShaderStage> shaders
    ) : GraphicsPipelineBuilder(layout) {
        for (const ShaderStage &stage : shaders) {
            addStage(stage);
        }
    }


}