#ifndef UNNECESSARYENGINE_CAMERAS_H
#define UNNECESSARYENGINE_CAMERAS_H

#include <unnecessary/systems/system.h>
#include <unnecessary/systems/graphics_systems.h>

namespace un {
    class CameraSystem : public un::System {
    private:
        un::Renderer* renderer;
    public:
        CameraSystem(Renderer* renderer);

        void step(un::World& world, f32 delta, un::JobWorker* worker) override;

        void describe(SystemDescriptor& descriptor) override;
    };
}


#endif //UNNECESSARYENGINE_CAMERAS_H
