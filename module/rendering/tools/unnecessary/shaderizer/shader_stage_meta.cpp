#include <unnecessary/shaderizer/shader_meta.h>

namespace un {
    void shader_stage_meta::uses_input(const std::string& input) {
        _usedInputs.emplace(input);
    }

    shader_stage_meta::shader_stage_meta(const std::string& name) : _name(name) {
    }

    const std::string& shader_stage_meta::get_name() const {
        return _name;
    }

    const std::set<std::string>& shader_stage_meta::get_used_inputs() const {
        return _usedInputs;
    }

    void shader_stage_meta::uses_vertex_attribute(
        const std::string& vertex,
        const std::string& modifier
    ) {
        _usedVertexAttributes.emplace_back(vertex, modifier);
    }

    const std::vector<input_usage>& shader_stage_meta::get_used_vertex_attributes() const {
        return _usedVertexAttributes;
    }

    bool shader_stage_meta::is_using_input_pack(const input_pack& scope) const {
        for (const auto& item : _usedInputs) {
            for (const auto& other : scope.get_inputs()) {
                if (other.get_name() == item) {
                    return true;
                }
            }
        }
        return false;
    }
}
