//
// Created by bruno on 12/07/2021.
//

#ifndef UNNECESSARYENGINE_LIGHTING_SYSTEM_H
#define UNNECESSARYENGINE_LIGHTING_SYSTEM_H

#include <unnecessary/systems/system.h>
#include <unnecessary/systems/world.h>
#include <unnecessary/misc/historic.h>
#include <unnecessary/graphics/drawable.h>
#include <unnecessary/graphics/lighting.h>
#include <unnecessary/graphics/descriptors/descriptor_allocator.h>
#include "graphics_systems.h"

namespace un {
    class LightingSystem : public un::SimpleSystem {
    private:
        u32 maxNumLightsPerObject;
        std::vector<un::PointLightData> runtimeScenePointLights;
        un::Renderer* renderer;
        bool lightsDirty;
        un::PrepareFrameGraphSystem* frameGraphSystem;
        u32 updateGPULightingDataCommandIndex;
        // GPU lighting data
        vk::DescriptorSet lightsDescriptorSet;
        un::ResizableBuffer lightsBuffer;
        un::DescriptorAllocator* lightsSetLayout;

    public:
        LightingSystem(u32 maxNumLightsPerObject, un::Renderer* device);

        void describe(SystemDescriptor& descriptor) override;

        void step(World& world, f32 delta, un::JobWorker* worker) override;

        size_t findSceneBufferSize();

        u32 getMaxNumLightsPerObject() const;

        const std::vector<un::PointLightData>& getRuntimeScenePointLights() const;

        const ResizableBuffer& getSceneLightingBuffer() const;

        u32 getUpdateGpuLightingDataCommandIndex() const;

        const vk::DescriptorSet& getLightsDescriptorSet() const;

        DescriptorAllocator* getLightsSetLayout() const;
    };
}


#endif //UNNECESSARYENGINE_LIGHTING_H
