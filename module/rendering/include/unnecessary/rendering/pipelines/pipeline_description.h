#ifndef UNNECESSARYENGINE_PIPELINE_DESCRIPTION_H
#define UNNECESSARYENGINE_PIPELINE_DESCRIPTION_H

#include <vulkan/vulkan.hpp>
#include <unnecessary/serialization/serialization_types.h>

#define SERIALIZE(...)

namespace un::rendering::bruh {
    struct OtherOptions {
        SERIALIZE()
        bool enabled;
    };
}
namespace un {

    struct PipelineDepthOptions {
        SERIALIZE()
        bool enabled;

        SERIALIZE()
        float depthMin;

        SERIALIZE()
        float depthMax;

        SERIALIZE()
        std::string name;
    };

    struct PipelineDescription {
        SERIALIZE()
        PipelineDepthOptions depth;
    };
}
#endif
