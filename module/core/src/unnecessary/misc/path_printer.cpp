#include "unnecessary/misc/path_printer.h"
#include "unnecessary/strings.h"

namespace un {
    std::string prettify(const std::filesystem::path& path) {
#if UN_PLATFORM_WINDOWS
        return "file:///" + un::to_string(path);
#else
        return "file://" + un::to_string(path);
#endif
    }

}