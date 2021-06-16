#ifndef UNNECESSARYENGINE_SYSTEM_H
#define UNNECESSARYENGINE_SYSTEM_H

#include <unnecessary/def.h>
#include <unnecessary/jobs/jobs.h>


namespace un {
    class World;

    struct System {
    public:
        virtual void step(World &world, f32 delta, un::JobWorker *worker) = 0;
    };
}
#endif
