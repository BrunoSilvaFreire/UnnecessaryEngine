#ifndef UNNECESSARYENGINE_SYSTEM_H
#define UNNECESSARYENGINE_SYSTEM_H

#include <unnecessary/def.h>
#include <unnecessary/systems/world.h>

namespace un {
    struct MultiJobSystem {
        virtual void step(World &world, f32 delta, JobSystem &jobSystem);
    };

    struct System {
    public:
        virtual void step(World &world, f32 delta) = 0;
    };
}
#endif
