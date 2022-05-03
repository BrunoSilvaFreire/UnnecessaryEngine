#include <unnecessary/rendering/pipelines/inputs.h>

namespace un {

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