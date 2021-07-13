//
// Created by bruno on 12/07/2021.
//

#ifndef UNNECESSARYENGINE_LIGHTING_SYSTEM_H
#define UNNECESSARYENGINE_LIGHTING_SYSTEM_H

#include <unnecessary/systems/system.h>
#include <unnecessary/systems/world.h>
#include <unnecessary/graphics/drawable.h>
#include <unnecessary/graphics/lighting.h>

namespace un {
    class LightingSystem : public un::System {
    private:
        u32 maxNumLightsPerObject;
        std::vector<un::PointLightData> runtimeScenePointLights;
        un::ResizableBuffer sceneLightingBuffer;
        un::Renderer* renderer;
        bool lightsDirty;
    public:
        LightingSystem(u32 maxNumLightsPerObject, un::Renderer* device);

        void step(World &world, f32 delta, un::JobWorker *worker) override;

        size_t findSceneBufferSize();
    };
}


#endif //UNNECESSARYENGINE_LIGHTING_H
