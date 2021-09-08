#include <unnecessary/graphics/buffers/command_buffer_graph.h>
#include <grapphs/flood.h>

namespace un {

    u32 CommandBufferGraph::enqueueCommandBuffer(
        vk::CommandBuffer buffer,
        vk::PipelineStageFlags waitMask
    ) {
        CommandBufferOperation vertex;
        vertex.cmdBuffer = buffer;
        vertex.waitMask = waitMask;
        u32 id = push(vertex);
        initializers.emplace(id);
        return id;
    }

    void CommandBufferGraph::addDependency(u32 from, u32 to) {
        initializers.erase(from);
        connect(
            from, to, {
                .type = un::CommandBufferDependency::Type::eUses
            }
        );
        connect(
            to, from, {
                .type = un::CommandBufferDependency::Type::eProvides
            }
        );
    }

    void CommandBufferGraph::submit(vk::Device device, vk::Queue graphicsQueue) {
        std::vector<vk::SubmitInfo> submitInfos;
        std::unordered_map<std::size_t, std::set<u32>> cmdBuffersGroupedByDependencies;
        std::unordered_map<std::size_t, std::set<u32>> groupHash2Dependencies;
        std::unordered_map<u32, vk::Semaphore> commandBuffer2Semaphore;
        for (u32 index : initializers) {
            std::set<u32> dependencies;
            bool anyoneDependsOnMe = false;
            for (auto[neighbor, edge] : edges_from(index)) {
                if (edge.type == un::CommandBufferDependency::Type::eUses) {
                    dependencies.emplace(neighbor);
                }
                if (edge.type == un::CommandBufferDependency::Type::eProvides) {
                    anyoneDependsOnMe = true;
                }
            }
            groupHash2Dependencies[0] = dependencies;
            cmdBuffersGroupedByDependencies[0].emplace(index);
            if (anyoneDependsOnMe) {
                vk::Semaphore semaphore = device.createSemaphore(
                    vk::SemaphoreCreateInfo(
                        static_cast<vk::SemaphoreCreateFlags>(0)
                    )
                );
                commandBuffer2Semaphore[index] = semaphore;
            }
        }
        gpp::flood(
            *this,
            initializers,
            [&](
                u32 origin,
                u32 index,
                const un::CommandBufferOperation& operation,
                const un::CommandBufferDependency& dependency
            ) {
                std::set<u32> dependencies;
                bool anyoneDependsOnMe = false;
                for (auto[neighbor, edge] : edges_from(index)) {
                    if (edge.type == un::CommandBufferDependency::Type::eUses) {
                        dependencies.emplace(neighbor);
                    }
                    if (edge.type == un::CommandBufferDependency::Type::eProvides) {
                        anyoneDependsOnMe = true;
                    }
                }
                std::size_t hash = hashDependencies(dependencies);
                groupHash2Dependencies[hash] = dependencies;
                cmdBuffersGroupedByDependencies[hash].emplace(index);
                if (anyoneDependsOnMe) {
                    vk::Semaphore semaphore = device.createSemaphore(
                        vk::SemaphoreCreateInfo(
                            static_cast<vk::SemaphoreCreateFlags>(0)
                        )
                    );
                    commandBuffer2Semaphore[index] = semaphore;
                }
            }

        );
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
                    auto it = commandBuffer2Semaphore.find(ownedVertex);
                    if (it == commandBuffer2Semaphore.end()) {
                        continue;
                    }
                    toSignal.emplace(it->second);
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
            std::sort(
                submitInfos.begin(),
                submitInfos.end(),
                [&](const vk::SubmitInfo& first, const vk::SubmitInfo& second) {
                    for (int j = 0 ; j < first.waitSemaphoreCount ; ++j) {
                        auto semaphore = first.pWaitSemaphores[j];
                        for (int k = 0 ; k < second.signalSemaphoreCount ; ++k) {
                            auto signalled = second.pSignalSemaphores[j];
                            if (semaphore == signalled) {
                                return false;
                            }
                        }
                    }
                    return true;
                }
            );
            if (!submitInfos.empty()) {
                graphicsQueue.submit(submitInfos);
            }
            graphicsQueue.waitIdle();
            for (auto[_, semaphore] : commandBuffer2Semaphore) {
                device.destroy(semaphore);
            }
            toDispose.dispose(device);
        }
    }

    std::size_t CommandBufferGraph::hashDependencies(const std::set<u32>& dependencies) {
        std::size_t hc = 0;
        hc ^= dependencies.size();
        for (u32 i : dependencies) {
            hc ^= i;
        }
        return hc;
    }


}