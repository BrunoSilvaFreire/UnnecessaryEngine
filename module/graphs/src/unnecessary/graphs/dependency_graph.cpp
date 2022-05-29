#include <unnecessary/graphs/dependency_graph.h>

namespace un {

    std::ostream& operator<<(std::ostream& stream, const un::DependencyType& dependencyType) {
        stream << un::to_string(dependencyType);
        return stream;
    }

    template<>
    std::string to_string(const DependencyType& type) {
        switch (type) {
            case eUses:
                return "uses";
                break;
            case eUsed:
                return "used";
                break;
        }
        return "unknown";
    }
}
