#ifndef UNNECESSARYENGINE_DRAWING_H
#define UNNECESSARYENGINE_DRAWING_H

#include <vulkan/vulkan.hpp>
#include <unnecessary/systems/system.h>
#include <unnecessary/graphics/drawable.h>
#include <unnecessary/graphics/renderer.h>
#include <unnecessary/graphics/descriptors/descriptor_set.h>
#include <unnecessary/graphics/descriptors/descriptor_allocator.h>
#include <unnecessary/jobs/jobs.h>
#include <unnecessary/systems/lighting.h>

namespace un {
    class DrawingSystem : public System {
    private:
        un::Renderer* renderer;

        un::DescriptorAllocator* objectSetLayout;
        un::DescriptorAllocator* cameraSetLayout;

        un::LightingSystem* lightingSystem{};
        un::PrepareFrameGraphSystem* frameGraphSystem{};
        un::DummyRenderingPipeline* renderingPipeline{};
    public:
        DrawingSystem(Renderer* renderer);

        void describe(SystemDescriptor& descriptor) override;

        void step(World& world, f32 delta, un::JobWorker* worker) override;


        const un::DescriptorAllocator* getCameraDescriptorSetAllocator() const;

        const un::DescriptorAllocator* getObjectDescriptorAllocator() const;


        un::DescriptorAllocator* getCameraDescriptorSetAllocator();

        un::DescriptorAllocator* getObjectDescriptorAllocator();


    };
}

#endif //UNNECESSARYENGINE_DRAWING_H
