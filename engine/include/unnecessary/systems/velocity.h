//
// Created by bruno on 14/06/2021.
//

#ifndef UNNECESSARYENGINE_VELOCITY_H
#define UNNECESSARYENGINE_VELOCITY_H

#include <unnecessary/systems/system.h>

namespace un {
    class VelocitySystem : public un::System {
    public:
        void step(World &world, f32 delta) override;

    };
}

#endif //UNNECESSARYENGINE_VELOCITY_H
