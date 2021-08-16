#ifndef UNNECESSARYENGINE_RENDERING_H
#define UNNECESSARYENGINE_RENDERING_H

#include <unnecessary/systems/system.h>
#include <unnecessary/graphics/drawable.h>
#include <unnecessary/graphics/systems/drawing.h>
#include <unnecessary/graphics/queue.h>
#include <unnecessary/graphics/buffers/command_buffer.h>
#include <unnecessary/components/common.h>

namespace un {
    struct PipelineBatch {
    private:
        un::Pipeline* pipeline;
        std::vector<entt::entity> batched;
    public:
        PipelineBatch(
            un::Pipeline* pipeline,
            std::vector<entt::entity> batched
        );

        un::Pipeline* getPipeline() const;

        const std::vector<entt::entity>& getBatched() const;

        bool operator<(const PipelineBatch& rhs) const;

        bool operator>(const PipelineBatch& rhs) const;

        bool operator<=(const PipelineBatch& rhs) const;

        bool operator>=(const PipelineBatch& rhs) const;
    };

    class ProjectionSystem : public System {
    public:
        void step(World& world, f32 delta, un::JobWorker* worker) override;
    };

    class RenderMeshSystem : public System {
    private:
        un::CommandBuffer buffer;
        un::DrawingSystem* drawingSystem;
    public:
        RenderMeshSystem(un::Renderer& renderer, un::DrawingSystem* system);

        void step(World& world, f32 delta, un::JobWorker* worker) override;
    };
}
#endif
