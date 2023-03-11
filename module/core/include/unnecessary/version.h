#ifndef UNNECESSARYENGINE_VERSION_H
#define UNNECESSARYENGINE_VERSION_H

#include <unnecessary/strings.h>
#include <unnecessary/def.h>

namespace un {
    struct version {
    private:
        u8 _major, _minor, _patch;
    public:
        version(u8 major, u8 minor, u8 patch);

        u8 get_major() const;

        u8 get_minor() const;

        u8 get_patch() const;
    };

    template<>
    std::string to_string(const version& ver);

}
#endif
