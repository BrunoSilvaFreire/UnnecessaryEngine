#ifndef UNNECESSARYENGINE_SHADER_H
#define UNNECESSARYENGINE_SHADER_H

#include <vulkan/vulkan.hpp>
#include <unnecessary/def.h>
#include <unnecessary/algorithm/layout.h>
#include <unnecessary/graphics/shader_stage.h>

namespace un {

    typedef std::string PipelineId;

    struct PipelineData {
        vk::PipelineCache cache;
        vk::PipelineLayout layout;
        vk::Pipeline pipeline;
    };


    class Pipeline {
    private:
        int order = 0;
        un::PipelineId id;
        un::PipelineData pipelineData;
    public:
        Pipeline(std::string name, un::PipelineData data);

        un::PipelineId getId() const;

        const vk::PipelineLayout &getPipelineLayout() const;

        const vk::Pipeline &getPipeline() const;

        int getOrder() const;

        void setOrder(int order);

    };
}
#endif
