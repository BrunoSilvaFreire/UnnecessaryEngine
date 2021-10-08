#ifndef UNNECESSARYENGINE_TRANSFORM_H
#define UNNECESSARYENGINE_TRANSFORM_H

#include <unnecessary/components/common.h>
#include <unnecessary/systems/system.h>
#include <unnecessary/systems/world.h>

namespace un {
    class TransformSystem : public SimpleSystem {
    public:
        void step(World& world, f32 delta, un::JobWorker* worker) override;

        void describe(SystemDescriptor& descriptor) override;
    };
}
#endif
