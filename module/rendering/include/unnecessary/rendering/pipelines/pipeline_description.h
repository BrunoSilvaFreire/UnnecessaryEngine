#ifndef UNNECESSARYENGINE_PIPELINE_DESCRIPTION_H
#define UNNECESSARYENGINE_PIPELINE_DESCRIPTION_H

#include <string>
#include <vector>
#include <unnecessary/rendering/pipelines/input_scope.h>

namespace un {
    struct VertexStreamUsageEntry {
        [[un::serialize(identifier)]]
        std::string attribute;
        [[un::serialize]]
        std::string usage; //Either in or out
    };

    struct VertexStreamEntry {
        [[un::serialize(identifier)]]
        std::string name;
        [[un::serialize]]
        std::string type;
    };

    struct PipelineStageDescription {
        [[un::serialize(identifier)]]
        std::string name;
        [[un::serialize(optional)]]
        std::vector<un::VertexStreamUsageEntry> vertexStream;
        [[un::serialize(optional)]]
        std::vector<std::string> inputsUsed;
    };

    struct PipelineDepthOptions {
        [[un::serialize(optional)]]
        bool enabled = true;

        [[un::serialize(optional)]]
        float depthMin = 0;
        [[un::serialize(optional)]]
        float depthMax = 1;
    };
    struct PipelineInput {
        [[un::serialize(identifier)]]
        std::string name;
        [[un::serialize(optional)]]
        un::InputScope scope = un::InputScope::eGlobal;
        [[un::serialize]]
        std::string type;
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

        [[un::serialize(
        optional
        )]]
        std::vector<un::PipelineInput> inputs;
    };
}
#endif
