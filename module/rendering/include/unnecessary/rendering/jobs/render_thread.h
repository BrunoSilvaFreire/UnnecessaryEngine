#ifndef UNNECESSARYENGINE_RENDER_THREAD_H
#define UNNECESSARYENGINE_RENDER_THREAD_H

#include <unnecessary/application/application.h>
#include <unnecessary/rendering/renderer.h>
#include <unnecessary/rendering/jobs/render_job.h>

namespace un {

    template<typename JobSystemType>
    class RenderThread : public un::Extension {
    private:
        bool _rendering = false;
        JobSystemType* _jobSystem;
        un::Renderer* _renderer;
        un::Thread* _thread;

        void schedulePasses(
            u32 framebufferIndex,
            un::FrameData& frameData,
            JobSystemType* jobSystem
        ) {
            auto graph = _renderer->getRenderGraph();
            un::FrameData data;
            data.renderPass = graph.getVulkanPass();

            data.framebuffer = graph.getFrameBuffer(framebufferIndex);
            un::GraphicsChain chain;
            std::unordered_map<u32, un::JobHandle> vertexIndex2JobHandle;
            std::unordered_map<u32, std::set<u32>> passesDependencies;
            graph.each(
                [&](u32 index, const un::RenderPass* const& pass) {
                    un::JobHandle handle = chain.immediately<un::RenderPassJob>(
                        &data,
                        pass
                    );
                    vertexIndex2JobHandle[index] = handle;
                },
                [&](u32 from, u32 to, const un::DependencyType& type) {
                    passesDependencies[to].emplace(from);
                }
            );
            for (auto[passIndex, passDependencies] : passesDependencies) {
                un::JobHandle passHandle = vertexIndex2JobHandle[passIndex];
                for (u32 dependencyIndex : passDependencies) {
                    un::JobHandle dependencyHandle = vertexIndex2JobHandle[dependencyIndex];
                    chain.after(dependencyHandle, passHandle);
                }
            }
            chain.finally(
                [&]() {
                    std::vector<vk::SubmitInfo> submits;
                    graph.each(
                        [&](u32 index, const un::RenderPass* const& pass) {
                            un::PassOutput& passOutput = frameData.passesOutputs[index];

                            std::vector<u32> dependencies = graph
                                .dependenciesOf(index)
                                .getDependencies();
                            std::vector<u32> dependants = graph
                                .dependantsOn(index)
                                .getDependencies();
                            std::vector<vk::Semaphore> waitSemaphores;
                            std::vector<vk::PipelineStageFlags> waitStageMask;
                            for (u32 dependency : dependencies) {
                                const auto& output = frameData.passesOutputs[dependency];
                                waitSemaphores.emplace_back(output.doneSemaphore);
                                waitStageMask.emplace_back(output.pipelineStage);
                            }
                            vk::SubmitInfo submit;
                            submit.setCommandBufferCount(1);
                            submit.setPCommandBuffers(&passOutput.passCommands);
                            if (!dependants.empty()) {
                                submit.setSignalSemaphoreCount(1);
                                submit.setPSignalSemaphores(&passOutput.doneSemaphore);
                            }
                            submit.setWaitSemaphores(waitSemaphores);
                            submit.setWaitDstStageMask(waitStageMask);
                            submits.push_back(submit);
                        },
                        [&](u32 from, u32 to, const un::DependencyType& type) {}
                    );

                    vk::Queue queue = _renderer->getDevice().getGraphics().getVulkan();
                    queue.submit(
                        submits
                    );
                    queue.waitIdle(); // TODO: Better sync
                }
            );
            chain.submit(jobSystem);
        }

        void renderThread() {
            while (_rendering) {
                const vk::Device& device = _renderer->getVirtualDevice();
                un::SwapChain& swapChain = _renderer->getSwapChain();
                vk::Semaphore semaphore = swapChain.acquireSemaphore();
                u32 imageIndex = device.acquireNextImageKHR(
                    swapChain.getSwapChain(),
                    1000,
                    semaphore
                ).value;
                un::FrameData frameData;
                schedulePasses(
                    imageIndex,
                    frameData,
                    _jobSystem
                );

            }
        }

    public:
        void start() {
            if (_rendering) {
                return;
            }
            _rendering = true;
            _thread = new un::Thread(
                "RenderThread",
                std::bind(&RenderThread::renderThread, this)
            );
        }

        void stop() {
            delete _thread;
            _rendering = false;
        }

        RenderThread(
            JobSystemType* jobSystem,
            Renderer* renderer
        ) : _jobSystem(jobSystem),
            _renderer(renderer),
            _thread(nullptr) {
        }

        void apply(Application& application) override {
            application.getOnStart() += [this]() {
                start();
            };
            application.getOnStop() += [this]() {
                stop();
            };
        }
    };
}
#endif
