#ifndef UNNECESSARYENGINE_PROJECTION_H
#define UNNECESSARYENGINE_PROJECTION_H

#include <unnecessary/systems/system.h>
#include <unnecessary/graphics/drawable.h>
#include <unnecessary/graphics/systems/drawing.h>
#include <unnecessary/graphics/queue.h>
#include <unnecessary/graphics/buffers/command_buffer.h>
#include <unnecessary/components/common.h>

namespace un {
    class ProjectionSystem : public System {
    public:
        void step(World& world, f32 delta, un::JobWorker* worker) override;

        void describe(SystemDescriptor& descriptor) override;
    };
}
#endif
