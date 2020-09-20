#ifndef UNNECESSARYENGINE_SYSTEM_H
#define UNNECESSARYENGINE_SYSTEM_H

#include <unnecessary/def.h>
#include <unnecessary/world.h>

namespace un {

    struct System {
    public:
        virtual void step(World &world, f32 delta) = 0;
    };
}
#endif
