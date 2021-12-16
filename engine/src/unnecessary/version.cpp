#include <unnecessary/version.h>
#include <sstream>

namespace un {

    Version::Version(u8 major, u8 minor, u8 patch) : major(major),
                                                     minor(minor),
                                                     patch(patch) {}

    u8 Version::getMajor() const {
        return major;
    }

    u8 Version::getMinor() const {
        return minor;
    }

    u8 Version::getPatch() const {
        return patch;
    }

    template<>
    std::string un::to_string(const Version& ver) {
        std::stringstream stream;
        stream << ver.getMajor() << '.' << ver.getMinor() << '.' << ver.getPatch();
        return stream.str();
    }

}