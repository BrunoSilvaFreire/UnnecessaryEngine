#ifndef UNNECESSARYENGINE_SHADER_META_H
#define UNNECESSARYENGINE_SHADER_META_H

#include <unordered_map>
#include <vector>
#include <string>
#include <set>
#include <unnecessary/rendering/pipelines/inputs.h>
#include <unnecessary/rendering/layout/vertex_layout.h>
#include <unnecessary/shaderizer/shader_stage_meta.h>

namespace un {
    struct vertex_attribute_meta {
    private:
        std::size_t _index;
        std::string _type;
        vertex_attribute _attribute;

    public:
        vertex_attribute_meta(
            size_t index,
            std::string type,
            const vertex_attribute& attribute
        );

        const std::string& get_type() const;

        size_t get_index() const;

        const vertex_attribute& get_attribute() const;
    };

    struct shader_input_meta {
    public:
        size_t index;
        input_scope scope;
        const rich_input* input;
    };

    class shader_meta {
    private:
        std::string _name;
        std::unordered_map<input_scope, input_pack> _inputs;
        std::vector<shader_stage_meta> _stages;
        std::vector<std::string> _vertexStreamTypes;
        vertex_layout _vertexStreamLayout;

    public:
        shader_meta(const std::string& name);

        void add_input(input_scope scope, rich_input&& input);

        void add_stage(const shader_stage_meta& meta);

        const vertex_layout& get_vertex_stream_layout() const;

        void set_vertex_stream_layout(
            const vertex_layout& newLayout,
            const std::vector<std::string>& vertexStreamTypes
        );

        vertex_attribute_meta get_vertex_attribute_meta(const std::string& vertexName) const;

        const std::string& get_name() const;

        const std::unordered_map<input_scope, input_pack>& get_inputs() const;

        const std::vector<shader_stage_meta>& get_stages() const;

        shader_input_meta get_shader_input_meta(std::string inputName) const;
    };
}
#endif
