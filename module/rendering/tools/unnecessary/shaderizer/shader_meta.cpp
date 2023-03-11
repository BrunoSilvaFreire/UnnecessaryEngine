#include <unnecessary/shaderizer/shader_meta.h>

namespace un {
    shader_meta::shader_meta(const std::string& name) : _name(name) {
    }

    void shader_meta::add_input(input_scope scope, rich_input&& input) {
        _inputs[scope] += std::move(input);
    }

    void shader_meta::add_stage(const shader_stage_meta& meta) {
        _stages.push_back(meta);
    }

    const std::string& shader_meta::get_name() const {
        return _name;
    }

    const std::unordered_map<input_scope, input_pack>& shader_meta::get_inputs() const {
        return _inputs;
    }

    const std::vector<shader_stage_meta>& shader_meta::get_stages() const {
        return _stages;
    }

    const vertex_layout& shader_meta::get_vertex_stream_layout() const {
        return _vertexStreamLayout;
    }

    void shader_meta::set_vertex_stream_layout(
        const vertex_layout& newLayout,
        const std::vector<std::string>& vertexStreamTypes
    ) {
        _vertexStreamLayout = newLayout;
        shader_meta::_vertexStreamTypes = vertexStreamTypes;
    }

    vertex_attribute_meta
    shader_meta::get_vertex_attribute_meta(const std::string& vertexName) const {
        std::size_t index = 0;
        while (index < _vertexStreamLayout.get_length()) {
            if (_vertexStreamLayout[index].get_name() == vertexName) {
                break;
            }
            index++;
        }
        if (index >= _vertexStreamLayout.get_length()) {
            std::string msg = "Unable to find vertex attribute with the specified name (";
            msg += vertexName;
            msg += ").";
            throw std::runtime_error(msg);
        }
        return vertex_attribute_meta(
            index,
            _vertexStreamTypes[index],
            _vertexStreamLayout[index]
        );
    }
    shader_input_meta shader_meta::get_shader_input_meta(std::string inputName) const {
        constexpr std::array<input_scope, 3> scopes = {
            global,
            pipeline,
            instance
        };
        for (input_scope scope : scopes) {
            auto found = _inputs.find(scope);
            if (found == _inputs.end()) {
                continue;
            }
            const input_pack& pack = found->second;
            const std::vector<rich_input>& packInputs = pack.get_inputs();
            for (std::size_t i = 0; i < packInputs.size(); ++i) {
                const rich_input& input = packInputs[i];
                if (input.get_name() != inputName) {
                    continue;
                }
                shader_input_meta meta;
                meta.index = i;
                meta.scope = scope;
                meta.input = &input;
                return meta;
            }
        }
        std::string msg = "Unable to find input with the given name (";
        msg += inputName;
        msg += ").";
        throw std::runtime_error(msg);
    }

    vertex_attribute_meta::vertex_attribute_meta(
        size_t index,
        std::string type,
        const vertex_attribute& attribute
    ) : _index(index), _type(type), _attribute(attribute) {
    }

    size_t vertex_attribute_meta::get_index() const {
        return _index;
    }

    const vertex_attribute& vertex_attribute_meta::get_attribute() const {
        return _attribute;
    }

    const std::string& vertex_attribute_meta::get_type() const {
        return _type;
    }

    const std::string& input_usage::get_name() const {
        return _name;
    }

    const std::string& input_usage::get_modifier() const {
        return _modifier;
    }

    input_usage::input_usage(
        const std::string& name,
        const std::string& modifier
    ) : _name(name), _modifier(modifier) {
    }
}
