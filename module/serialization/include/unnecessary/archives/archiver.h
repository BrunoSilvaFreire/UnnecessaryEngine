#ifndef UNNECESSARYENGINE_ARCHIVER_H
#define UNNECESSARYENGINE_ARCHIVER_H

#include <unnecessary/serialization/serialized.h>
#include "unnecessary/memory/membuf.h"

namespace un {
    class Archiver {
        virtual un::Buffer write(const un::Serialized& serialized) = 0;

        virtual void read(
            const un::Buffer& buffer,
            un::Serialized& into
        ) = 0;
    };
}
#endif
