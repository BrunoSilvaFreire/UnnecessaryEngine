#include <unnecessary/version.h>
#include <sstream>

namespace un {
    version::version(u8 major, u8 minor, u8 patch) : _major(major),
                                                     _minor(minor),
                                                     _patch(patch) {
    }

    u8 version::get_major() const {
        return _major;
    }

    u8 version::get_minor() const {
        return _minor;
    }

    u8 version::get_patch() const {
        return _patch;
    }

    template<>
    std::string to_string(const version& ver) {
        std::stringstream stream;
        stream << ver.get_major() << '.' << ver.get_minor() << '.' << ver.get_patch();
        return stream.str();
    }
}
