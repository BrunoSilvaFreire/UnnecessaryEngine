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
    class PrepareFrameGraphSystem : public un::System {
    private:
        un::Renderer* renderer;
        un::CommandBufferGraph commandGraph;
        un::DummyRenderingPipeline* renderingPipeline;
        std::set<u32> preparationCommands;

        u8 currentFramebufferIndex{};
        u8 latestFramebufferIndex{};
        std::vector<vk::Framebuffer> framebuffers;
        vk::Semaphore imageAvailableSemaphore;

        vk::Framebuffer nextFramebuffer(u32* framebufferIndexResult);

    public:
        PrepareFrameGraphSystem(Renderer* renderer);

        void describe(SystemDescriptor& descriptor) override;

        void step(World& world, f32 delta, un::JobWorker* worker) override;

        CommandBufferGraph& getCommandGraph();

        vk::Framebuffer getCurrentFramebuffer();

        vk::Framebuffer getCurrentFramebuffer(u32* pIndex);

        u32 enqueuePreparationPhase(
            const vk::CommandBuffer& buffer,
            vk::PipelineStageFlagBits bits
        );

        friend class DispatchFrameGraphSystem;

    };

    class DispatchFrameGraphSystem : public un::System {
    private:
        glm::vec4 clearColor;
        un::PrepareFrameGraphSystem* graphSystem{};
        un::DummyRenderingPipeline* renderingPipeline{};
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