#include <unnecessary/shaderizer/inputs/shader_input.h>

namespace un {
    rich_input::rich_input(
        const std::string& name,
        const std::string& type
    ) : _name(name),
        _type(type) {
    }

    const std::string& rich_input::get_name() const {
        return _name;
    }

    const std::string& rich_input::get_type() const {
        return _type;
    }

    void input_pack::operator+=(rich_input&& other) {
        _inputs.emplace_back(std::move(other));
    }

    const std::vector<rich_input>& input_pack::get_inputs() const {
        return _inputs;
    }

    std::vector<rich_input>& input_pack::get_inputs() {
        return _inputs;
    }
}
