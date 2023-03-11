#ifndef UNNECESSARYENGINE_PIPELINE_DESCRIPTION_H
#define UNNECESSARYENGINE_PIPELINE_DESCRIPTION_H

#include <string>
#include <vector>
#include <unnecessary/rendering/pipelines/input_scope.h>

namespace un {
    struct vertex_stream_usage_entry {
        [[un::serialize(identifier)]]
        std::string attribute;
        [[un::serialize]]
        std::string usage; //Either in or out
    };

    struct vertex_stream_entry {
        [[un::serialize(identifier)]]
        std::string name;
        [[un::serialize]]
        std::string type;
    };

    struct pipeline_stage_description {
        [[un::serialize(identifier)]]
        std::string name;
        [[un::serialize(optional)]]
        std::vector<vertex_stream_usage_entry> vertex_stream;
        [[un::serialize(optional)]]
        std::vector<std::string> inputs_used;
    };

    struct pipeline_depth_options {
        [[un::serialize(optional)]]
        bool enabled = true;

        [[un::serialize(optional)]]
        float depthMin = 0;
        [[un::serialize(optional)]]
        float depthMax = 1;
    };

    struct pipeline_input {
        [[un::serialize(identifier)]]
        std::string name;
        [[un::serialize(optional)]]
        input_scope scope = global;
        [[un::serialize]]
        std::string type;
    };

    struct pipeline_description {
        [[un::serialize(optional)]]
        pipeline_depth_options depth;

        [[un::serialize]]
        std::vector<pipeline_stage_description> stages;

        [[un::serialize]]
        std::vector<pipeline_input> inputs;
        [[un::serialize]]
        std::vector<pipeline_input> vertex_stream;
    };
}
#endif
