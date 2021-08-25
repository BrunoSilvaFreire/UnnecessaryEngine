//
// Created by brunorbsf on 24/08/2021.
//

#ifndef UNNECESSARYENGINE_GAMEPLAY_H
#define UNNECESSARYENGINE_GAMEPLAY_H

#include <unnecessary/systems/system.h>

namespace un {
    class PathRunningSystem : public un::System {
    public:
        void step(World& world, f32 delta, un::JobWorker* worker) override;

        void describe(SystemDescriptor& descriptor) override;
    };
}


#endif //UNNECESSARYENGINE_GAMEPLAY_H
