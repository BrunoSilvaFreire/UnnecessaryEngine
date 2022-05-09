#ifndef UNNECESSARYENGINE_PIPELINE_DESCRIPTION_H
#define UNNECESSARYENGINE_PIPELINE_DESCRIPTION_H

#include <vulkan/vulkan.hpp>
#include <string>

namespace un {
    struct VertexStreamUsageEntry {
        std::string attribute;
        std::string usage; //Either in or out
    };


    struct PipelineStageDescription {
        std::vector<un::VertexStreamUsageEntry> vertexStream;
        std::vector<std::string> inputsUsed;
        std::string name;
    };

    struct PipelineDepthOptions {
        [[un::serialize(optional, defaultValue=true)]]
        bool enabled = true;

        [[un::serialize(optional)]]
        float depthMin = 0;
        [[un::serialize(optional)]]
        float depthMax = 1;


    };

    struct PipelineDescription {
        [[un::serialize(optional)]]
        PipelineDepthOptions depth;

        std::vector<un::PipelineStageDescription> pipelineStageDescription;
    };
}
#endif
