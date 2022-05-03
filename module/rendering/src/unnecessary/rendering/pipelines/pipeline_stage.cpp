#include <unnecessary/rendering/pipelines/pipeline_stage.h>

namespace un {

    PipelineStage::PipelineStage(vk::ShaderStageFlagBits stage, const vk::ShaderModule& module)
        : module(module), stage(stage) { }

    const vk::ShaderModule& PipelineStage::getModule() const {
        return module;
    }

    vk::ShaderStageFlagBits PipelineStage::getStageFlags() const {
        return stage;
    }
}