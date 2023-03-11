#include <unnecessary/rendering/pipelines/pipeline_stage.h>

namespace un {
    pipeline_stage::pipeline_stage(
        vk::ShaderStageFlagBits stage, const vk::ShaderModule&
    module)
    : module(module),
    stage(stage)
        {
        }

    const vk::ShaderModule& pipeline_stage::get_module() const {
        return module;
    }

    vk::ShaderStageFlagBits pipeline_stage::get_stage_flags() const {
        return _stage;
    }
}
