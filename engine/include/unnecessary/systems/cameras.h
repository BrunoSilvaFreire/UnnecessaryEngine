#ifndef UNNECESSARYENGINE_CAMERAS_H
#define UNNECESSARYENGINE_CAMERAS_H

#include <unnecessary/systems/world.h>

namespace un {
    class CameraSystem : public un::System {
    private:
        un::Renderer * renderer;
    public:
        CameraSystem(Renderer* renderer);

        void step(un::World& world, f32 delta, un::JobWorker* worker) override;
    };
}


#endif //UNNECESSARYENGINE_CAMERAS_H
