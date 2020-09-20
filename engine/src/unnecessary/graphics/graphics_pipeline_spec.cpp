#include <unnecessary/graphics/pipeline_spec.h>

namespace un {
    void GraphicsPipelineSpecification::with3DModelInputAssembly() {
        inputAssembly.emplace(
                (vk::PipelineInputAssemblyStateCreateFlags) 0,
                vk::PrimitiveTopology::eTriangleList
        );
    }

    template<typename T>
    T *getOrNull(std::optional<T> &optional) {
        if (optional) {
            return optional.operator->();
        }
        return nullptr;
    }

    void GraphicsPipelineSpecification::addStage(const vk::PipelineShaderStageCreateInfo &info) {
        stages.push_back(info);
    }

    vk::Pipeline GraphicsPipelineSpecification::create(const vk::Device &device) {
        return device.createGraphicsPipeline(
                nullptr,
                vk::GraphicsPipelineCreateInfo(
                        (vk::PipelineCreateFlags) 0,
                        stages,
                        getOrNull(vertexInput),
                        getOrNull(inputAssembly),
                        getOrNull(tesselation),
                        getOrNull(viewport),
                        getOrNull(rasterization),
                        getOrNull(multisample),
                        getOrNull(depthStencil),
                        getOrNull(colorBlend),
                        getOrNull(dynamicState)
                )
        );
    }

    void GraphicsPipelineSpecification::addStage(const ShaderStage &stage) {
        stages.emplace_back(
                (vk::PipelineShaderStageCreateFlags) 0,
                stage.getBits(),
                stage.getModule(),
                stage.getName().c_str(),
                nullptr
        );
    }

    GraphicsPipelineSpecification::GraphicsPipelineSpecification() = default;
}