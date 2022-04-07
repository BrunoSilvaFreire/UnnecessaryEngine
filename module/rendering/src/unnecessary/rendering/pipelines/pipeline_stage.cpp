#include <unnecessary/rendering/pipelines/pipeline_stage.h>

namespace un {

    PipelineStage::PipelineStage(
        const vk::ShaderModule& module
    ) : module(module) { }
}