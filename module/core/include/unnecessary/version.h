#ifndef UNNECESSARYENGINE_VERSION_H
#define UNNECESSARYENGINE_VERSION_H

#include <unnecessary/strings.h>
#include <unnecessary/def.h>

namespace un {
    struct Version {
    private:
        u8 major, minor, patch;
    public:
        Version(u8 major, u8 minor, u8 patch);

        u8 getMajor() const;

        u8 getMinor() const;

        u8 getPatch() const;
    };

    template<>
    std::string to_string(const Version& ver);

}
#endif
