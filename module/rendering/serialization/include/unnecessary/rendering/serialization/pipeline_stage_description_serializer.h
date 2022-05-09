#ifndef UNNECESSARYENGINE_PIPELINE_STAGE_DESCRIPTION_SERIALIZER_H
#define UNNECESSARYENGINE_PIPELINE_STAGE_DESCRIPTION_SERIALIZER_H

#include <unnecessary/serialization/serializer.h>
#include <unnecessary/rendering/pipelines/pipeline_description.h>

namespace un {
    namespace serialization {
        template<>
        inline void serialize(const un::PipelineStageDescription& value, un::Serialized& into) {
            un::Serialized vs;
            for (const auto& item : value.vertexStream) {
                vs.set(item.attribute, item.usage);
            }
            into.set("vertex_stream", vs);

            un::SerializedArray<std::string> usedDescriptors;
            into.set("inputs", std::move(usedDescriptors));
        }

        template<>
        inline un::PipelineStageDescription deserialize(un::Serialized& from) {
            un::Serialized vs;
            std::vector<un::VertexStreamUsageEntry> vertexStream;
            if (!from.try_get("vertex_stream", vs)) {
                for (const auto& item : vs.getNamedProperties()) {
                    un::VertexStreamUsageEntry entry;
                    entry.attribute = item.first;
                    entry.usage = item.second->as<un::SerializedString>();
                    vertexStream.push_back(std::move(entry));
                }
            }
            std::vector<std::string> usedDescriptors;
            un::SerializedArray<std::string> inputs;
            if (!from.try_get("inputs", inputs)) {
                usedDescriptors = inputs;
            }
            un::PipelineStageDescription description;
            description.vertexStream = vertexStream;
            description.inputsUsed = usedDescriptors;
            return description;
        }

        inline void pipeline_stages_serializer(
            const std::vector<un::PipelineStageDescription>& value,
            un::Serialized& into
        ) {
            for (const auto& item : value) {
                un::Serialized sStage;
                un::serialization::serialize(item, sStage);
                into.set(item.name, sStage);
            }
        }

        inline std::vector<un::PipelineStageDescription> pipeline_stages_deserializer(
            un::Serialized& from
        ) {
            std::vector<un::PipelineStageDescription> stages;
            for (const auto& [key, node] : from.getNamedProperties()) {
                auto stage = un::serialization::deserialize<un::PipelineStageDescription>(node->as<un::Serialized>());
                stage.name = key;
                stages.push_back(std::move(stage));
            }
            return stages;
        }
    }

    class PipelineStageDescriptionSerializer : public un::Serializer<un::PipelineStageDescription> {

    };

}
#endif
