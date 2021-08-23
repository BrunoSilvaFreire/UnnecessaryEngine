#include <unnecessary/graphics/buffers/command_buffer_graph.h>
namespace un {

    u32 CommandBufferGraph::enqueueCommandBuffer(
        vk::CommandBuffer buffer,
        vk::PipelineStageFlags waitMask
    ) {
        CommandBufferOperation vertex;
        vertex.cmdBuffer = buffer;
        vertex.waitMask = waitMask;
        return push(vertex);
    }

    void CommandBufferGraph::addDependency(u32 from, u32 to) {
        connect(
            from, to, {
                .type = un::CommandBufferDependency::Type::eProvides
            }
        );
        connect(
            to, from, {
                .type = un::CommandBufferDependency::Type::eUses
            }
        );
    }
}