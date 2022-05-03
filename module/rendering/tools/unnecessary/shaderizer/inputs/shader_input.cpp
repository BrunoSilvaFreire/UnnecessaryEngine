#include <unnecessary/shaderizer/inputs/shader_input.h>

namespace un {
    RichInput::RichInput(
        const std::string& name,
        const std::string& type
    ) : name(name),
        type(type) { }

    const std::string& RichInput::getName() const {
        return name;
    }

    const std::string& RichInput::getType() const {
        return type;
    }

    void InputPack::operator+=(RichInput&& other) {
        inputs.emplace_back(std::move(other));
    }

    const std::vector<RichInput>& InputPack::getInputs() const {
        return inputs;
    }

    std::vector<RichInput>& InputPack::getInputs() {
        return inputs;
    }

}