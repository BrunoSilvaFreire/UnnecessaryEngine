#include "unnecessary/misc/path_printer.h"
#include "unnecessary/strings.h"

namespace un {
    std::string uri(const std::filesystem::path& path) {
#if UN_PLATFORM_WINDOWS
        return "file:///" + to_string(_path);
#else
        return "file://" + un::to_string(path);
#endif
    }
    std::string prettify(const std::filesystem::path& path) {
        return un::uri(path);
    }
}
