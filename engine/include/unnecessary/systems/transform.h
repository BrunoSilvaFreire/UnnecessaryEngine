#ifndef UNNECESSARYENGINE_TRANSFORM_H
#define UNNECESSARYENGINE_TRANSFORM_H

#include <unnecessary/systems/system.h>
#include <unnecessary/components/common.h>

namespace un {
    class TransformSystem : public System {
    public:
        void step(World &world, f32 delta) override;
    };
}
#endif
