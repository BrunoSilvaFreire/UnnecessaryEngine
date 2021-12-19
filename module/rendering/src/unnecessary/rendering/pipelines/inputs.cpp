#include <unnecessary/rendering/pipelines/inputs.h>

namespace un {
    un::Input::Input(
        const std::string& name,
        const std::string& type
    ) : name(name),
        type(type) {}

    const std::string& Input::getName() const {
        return name;
    }

    const std::string& Input::getType() const {
        return type;
    }

    template<>
    std::string to_string(const InputScope& scope) {
        switch (scope) {
            case eGlobal:
                return "global";
            case ePipeline:
                return "pipeline";
            case eInstance:
                return "instance";
        }
        return "unknown_scope";
    }
}