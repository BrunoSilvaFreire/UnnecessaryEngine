#ifndef UNNECESSARYENGINE_PIPELINE_DESCRIPTION_H
#define UNNECESSARYENGINE_PIPELINE_DESCRIPTION_H

#include <vulkan/vulkan.hpp>

namespace un {
    class PipelineDepthOptions {
        bool enabled;
        float depthMin;
        float depthMax;
    };
}
#endif
