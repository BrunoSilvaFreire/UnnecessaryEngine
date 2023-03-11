#include <unnecessary/graphs/dependency_graph.h>

namespace un {
    std::ostream& operator<<(std::ostream& stream, const dependency_type& dependencyType) {
        stream << to_string(dependencyType);
        return stream;
    }

    template<>
    std::string to_string(const dependency_type& type) {
        switch (type) {
            case uses:
                return "uses";
                break;
            case used:
                return "used";
                break;
        }
        return "unknown";
    }
}
