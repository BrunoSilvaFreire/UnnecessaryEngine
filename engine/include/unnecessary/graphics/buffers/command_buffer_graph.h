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

    class CommandBufferGraph
        : public gpp::AdjacencyList<CommandBufferOperation, CommandBufferDependency, u32> {
    public:

        u32
        enqueueCommandBuffer(vk::CommandBuffer buffer, vk::PipelineStageFlags waitMask);

        void addDependency(u32 from, u32 to);

        static std::size_t hashDependencies(const std::set<u32>& dependencies) {
            std::size_t hc = 0;
            for (u32 i : dependencies) {
                hc ^= i;
            }
            return hc;
        }

        void submit(vk::Device device, vk::Queue graphicsQueue) {
            std::vector<vk::SubmitInfo> submitInfos;
            std::unordered_map<std::size_t, std::vector<u32>> cmdBuffersGroupedByDependencies;
            std::unordered_map<std::size_t, std::set<u32>> groupHash2Dependencies;
            std::unordered_map<u32, vk::Semaphore> commandBuffer2Semaphore;
            for (auto[vertex, index] : all_vertices()) {
                std::set<u32> dependencies;
                for (auto[neighbor, edge] : edges_from(index)) {
                    if (edge.type == un::CommandBufferDependency::Type::eUses) {
                        dependencies.emplace(neighbor);
                    }
                }
                std::size_t hash = hashDependencies(dependencies);
                groupHash2Dependencies[hash] = dependencies;
                auto& buffers = cmdBuffersGroupedByDependencies[hash];
                buffers.emplace_back(index);
                vk::Semaphore semaphore = device.createSemaphore(
                    vk::SemaphoreCreateInfo(
                        static_cast<vk::SemaphoreCreateFlags>(0)
                    )
                );
                commandBuffer2Semaphore[index] = semaphore;
            }
            {
                un::Aggregator<vk::CommandBuffer> buffersAggregator;
                un::Aggregator<vk::Semaphore> toWait;
                un::Aggregator<vk::Semaphore> toSignal;
                un::Aggregator<vk::PipelineStageFlags> waitDstStageMask;
                toSignal.reserve(4);
                toWait.reserve(4);
                waitDstStageMask.reserve(4);
                buffersAggregator.reserve(4);
                for (auto[hash, vertices] : cmdBuffersGroupedByDependencies) {
                    buffersAggregator.begin();
                    toWait.begin();
                    toSignal.begin();
                    waitDstStageMask.begin();
                    buffersAggregator.reserve(vertices.size());
                    for (u32 vertex : vertices) {
                        buffersAggregator.emplace(
                            CommandBufferGraph::vertex(vertex)->cmdBuffer
                        );
                    }
                    const std::set<u32>& dependencies = groupHash2Dependencies[hash];


                    vk::SubmitInfo info;
                    info.setCommandBuffers(buffersAggregator);
                    for (u32 ownedVertex : vertices) {
                        toSignal.emplace(commandBuffer2Semaphore[ownedVertex]);
                    }
                    for (u32 dependency : dependencies) {
                        toWait.emplace(commandBuffer2Semaphore[dependency]);
                        waitDstStageMask.emplace(
                            CommandBufferGraph::vertex(dependency)->waitMask
                        );
                    }
                    info.setSignalSemaphores(toSignal);
                    info.setWaitSemaphores(toWait);
                    info.setWaitDstStageMask(waitDstStageMask);
                    submitInfos.push_back(info);
                }
                if (!submitInfos.empty()) {
                    graphicsQueue.submit(submitInfos);
                }
                graphicsQueue.waitIdle();
                for (auto[_, semaphore]:commandBuffer2Semaphore) {
                    device.destroy(semaphore);
                }
            }
        }
    };
}
#endif
