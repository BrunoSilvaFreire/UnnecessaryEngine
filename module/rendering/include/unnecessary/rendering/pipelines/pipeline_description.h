#ifndef UNNECESSARYENGINE_PIPELINE_DESCRIPTION_H
#define UNNECESSARYENGINE_PIPELINE_DESCRIPTION_H

#include <vulkan/vulkan.hpp>
#include <unnecessary/serialization/serialization_types.h>
#define SERIALIZE(...)

namespace un {

    struct PipelineDepthOptions {
        SERIALIZE()
        bool enabled;

        SERIALIZE()
        float depthMin;

        SERIALIZE()
        float depthMax;
    };

    struct PipelineDescription {
        SERIALIZE()
        PipelineDepthOptions depth;
    };
}
#endif
