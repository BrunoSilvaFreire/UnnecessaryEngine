#include <unnecessary/rendering/pipelines/input_scope.h>

namespace un {
    template<>
    std::string to_string(const input_scope& scope) {
        switch (scope) {
            case global:
                return "global";
            case pipeline:
                return "pipeline";
            case instance:
                return "instance";
        }
        return "unknown_scope";
    }
}
