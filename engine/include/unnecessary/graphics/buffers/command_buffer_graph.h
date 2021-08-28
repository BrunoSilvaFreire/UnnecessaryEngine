#ifndef UNNECESSARYENGINE_COMMAND_BUFFER_GRAPH_H
#define UNNECESSARYENGINE_COMMAND_BUFFER_GRAPH_H

#include <vulkan/vulkan.hpp>
#include <grapphs/adjacency_list.h>
#include <unnecessary/def.h>
#include <set>
#include <string>
#include <unnecessary/misc/aggregator.h>

namespace un {
    struct CommandBufferOperation {
        vk::PipelineStageFlags waitMask;
        vk::CommandBuffer cmdBuffer;
    };

    struct CommandBufferDependency {
        enum Type {
            eUses,
            eProvides
        };
        Type type;
    };

    class CommandBufferGraph : public gpp::AdjacencyList<CommandBufferOperation, CommandBufferDependency, u32> {
    private:
        static std::size_t hashDependencies(const std::set<u32> &dependencies);
        std::set<u32> initializers;
    public:
        using VertexType = un::CommandBufferOperation;
        using EdgeType = un::CommandBufferDependency;
        using IndexType = u32;
        u32 enqueueCommandBuffer(vk::CommandBuffer buffer, vk::PipelineStageFlags waitMask);

        void addDependency(u32 from, u32 to);

        void submit(vk::Device device, vk::Queue graphicsQueue);
    };
}
#endif
