#include <unnecessary/rendering/pipelines/pipeline.h>

namespace un {

    vk::Pipeline Pipeline::operator*() const {
        return pipeline;
    }

    const vk::PipelineCache& Pipeline::getCache() const {
        return cache;
    }

    const vk::PipelineLayout& Pipeline::getLayout() const {
        return layout;
    }

    Pipeline::Pipeline(
        const vk::Pipeline& pipeline,
        const vk::PipelineCache& cache,
        const vk::PipelineLayout& layout
    ) : pipeline(pipeline), cache(cache), layout(layout) { }
}