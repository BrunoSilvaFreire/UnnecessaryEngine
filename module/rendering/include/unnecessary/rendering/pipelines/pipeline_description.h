#ifndef UNNECESSARYENGINE_PIPELINE_DESCRIPTION_H
#define UNNECESSARYENGINE_PIPELINE_DESCRIPTION_H

#include <string>
#include <vector>

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
        [[un::serialize(optional)]]
        bool enabled = true;

        [[un::serialize(optional)]]
        float depthMin = 0;
        [[un::serialize(optional)]]
        float depthMax = 1;
    };

    struct PipelineDescription {
        [[un::serialize(optional)]]
        PipelineDepthOptions depth;
        [[un::serialize(
            optional,
            serializer=un::serialization::pipeline_stages_serializer,
            deserializer=un::serialization::pipeline_stages_deserializer
        )]]
        std::vector<un::PipelineStageDescription> stages;
    };
}
#endif
