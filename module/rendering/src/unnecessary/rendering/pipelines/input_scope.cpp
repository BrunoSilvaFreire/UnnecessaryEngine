#include <unnecessary/rendering/pipelines/input_scope.h>

namespace un {

    template<>
    std::string to_string(const un::InputScope& scope) {
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