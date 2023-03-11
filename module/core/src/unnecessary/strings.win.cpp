#include <unnecessary/strings.h>

namespace un {
    template<>
    std::string un::to_string(const std::filesystem::path& value) {
        auto path = value.string();
        std::replace(path.begin(), path.end(), '\\', '/');
        return path;
    }
}
