#ifndef UNNECESSARYENGINE_GRAPHICS_SYSTEMS_H
#define UNNECESSARYENGINE_GRAPHICS_SYSTEMS_H

#include <unnecessary/systems/system.h>
#include <unnecessary/graphics/buffers/command_buffer_graph.h>
#include <unnecessary/graphics/buffers/command_buffer.h>
#include <unnecessary/misc/aggregator.h>
#include <unordered_set>
#include <unordered_map>
#include <functional>

namespace un {
    class PrepareFrameGraphSystem : public un::SimpleSystem {
    private:
        un::Renderer* renderer;
        un::CommandBufferGraph commandGraph;
        un::PhongRenderingPipeline* renderingPipeline;
        std::set<u32> preparationCommands;

        u32 currentFramebufferIndex{};
        vk::Semaphore imageAvailableSemaphore;

    public:
        PrepareFrameGraphSystem(Renderer* renderer);

        void describe(SystemDescriptor& descriptor) override;

        void step(World& world, f32 delta, un::JobWorker* worker) override;

        CommandBufferGraph& getCommandGraph();

        vk::Framebuffer getCurrentFramebuffer();

        vk::Framebuffer getCurrentFramebuffer(u32* pIndex);

        u32 enqueuePreparationPhase(
            const vk::CommandBuffer& buffer,
            vk::PipelineStageFlagBits bits = vk::PipelineStageFlagBits::eTopOfPipe
        );

        friend class DispatchFrameGraphSystem;

    };

    class DispatchFrameGraphSystem : public un::SimpleSystem {
    private:
        un::PrepareFrameGraphSystem* graphSystem{};
        un::PhongRenderingPipeline* renderingPipeline{};
        un::Renderer* renderer;

    public:
        DispatchFrameGraphSystem(
            Renderer* renderer
        );

        void describe(SystemDescriptor& descriptor) override;

        void step(World& world, f32 delta, un::JobWorker* worker) override;

    };
}
#endif