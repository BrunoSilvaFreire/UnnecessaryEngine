#include <unnecessary/def.h>
#include <unnecessary/graphics/shading/shader.h>

#include <utility>

namespace un {

    Pipeline::Pipeline(
            std::string name,
            un::PipelineData data
    ) : id(std::move(name)),
        pipelineData(data) {

    }

    PipelineId Pipeline::getId() const {
        return id;
    }

    const vk::PipelineLayout &Pipeline::getPipelineLayout() const {
        return pipelineData.layout;
    }

    const vk::Pipeline &Pipeline::getPipeline() const {
        return pipelineData.pipeline;
    }

    int Pipeline::getOrder() const {
        return order;
    }

    void Pipeline::setOrder(int order) {
        Pipeline::order = order;
    }


}