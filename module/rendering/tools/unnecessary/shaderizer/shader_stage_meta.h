//
// Created by bruno on 02/05/2022.
//

#ifndef UNNECESSARYENGINE_SHADER_STAGE_META_H
#define UNNECESSARYENGINE_SHADER_STAGE_META_H

#include <unordered_map>
#include <vector>
#include <string>
#include <set>
#include <unnecessary/rendering/pipelines/inputs.h>
#include <unnecessary/rendering/layout/vertex_layout.h>
#include <unnecessary/shaderizer/inputs/shader_input.h>

namespace un {
    struct depth_info {
        bool enabled;
    };

    class shader_stage_meta {
    private:
        std::string _name;
        std::set<std::string> _usedInputs;
        std::vector<input_usage> _usedVertexAttributes;

    public:
        shader_stage_meta(const std::string& name);

        void uses_input(const std::string& input);

        void uses_vertex_attribute(const std::string& vertex, const std::string& modifier);

        const std::string& get_name() const;

        const std::set<std::string>& get_used_inputs() const;

        const std::vector<input_usage>& get_used_vertex_attributes() const;

        bool is_using_input_pack(const input_pack& scope) const;
    };
}

#endif //UNNECESSARYENGINE_SHADER_STAGE_META_H
