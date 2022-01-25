#ifndef UNNECESSARYENGINE_RENDER_THREAD_H
#define UNNECESSARYENGINE_RENDER_THREAD_H

#include <unnecessary/application/application.h>
#include <unnecessary/rendering/renderer.h>
#include <unnecessary/rendering/jobs/render_job.h>
#include "unnecessary/misc/benchmark.h"

namespace un {

    template<typename JobSystemType>
    class RenderThread : public un::Extension {
    private:
        bool _rendering = false;
        std::size_t _frame;
        JobSystemType* _jobSystem;
        un::Renderer* _renderer;
        un::Thread* _thread;
        std::vector<un::FrameData> _inFlightFrames;

        void schedulePasses(
            u32 framebufferIndex,
            un::FrameData& frameData,
            JobSystemType* jobSystem,
            un::SwapChain::ChainSynchronizer& synchronizer
        ) {
            const RenderGraph& graph = _renderer->getRenderGraph();

            un::GraphicsChain chain;
            std::unordered_map<u32, un::JobHandle> vertexIndex2JobHandle;
            std::unordered_map<u32, std::set<u32>> passesDependencies;
            graph.each(
                [&](u32 index, const un::RenderPass* const& pass) {
                    un::JobHandle handle = chain.immediately<un::RecordPassJob>(
                        &frameData,
                        pass,
                        index
                    );
                    std::string jobName = "Record Pass: \"";
                    jobName += pass->getName();
                    jobName += '"';
                    chain.setName(handle, jobName);
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
                "Submit Render Commands",
                [&, framebufferIndex]() {
                    submitCommands(
                        framebufferIndex,
                        frameData,
                        synchronizer,
                        graph,
                        _renderer->getSwapChain().getSwapChain()
                    );
                }
            );
            chain.submit(jobSystem);
        }

        void submitCommands(
            u32 framebufferIndex,
            const FrameData& frameData,
            SwapChain::ChainSynchronizer& synchronizer,
            const RenderGraph& graph,
            vk::SwapchainKHR swapchain
        ) const {
            LOG(INFO) << "Submitted " << synchronizer.getInnerIndex();
            std::vector<vk::SubmitInfo> submits;
            graph.each(
                [&](u32 index, const RenderPass* const& pass) {
                    const PassOutput& passOutput = frameData.passesOutputs[index];

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
                    if (!waitSemaphores.empty()) {
                        submit.setWaitSemaphores(waitSemaphores);
                        submit.setWaitDstStageMask(waitStageMask);
                    }
                    submits.push_back(submit);
                },
                [&](u32 from, u32 to, const DependencyType& type) {}
            );

            vk::Queue queue = *_renderer->getDevice().getGraphics();
            queue.submit(
                submits,
                synchronizer.getFence()
            );
            std::array<vk::Semaphore, 1> imageReady(
                {
                    synchronizer.getImageReady()
                }
            );
            std::array<u32, 1> imageIndices(
                {
                    framebufferIndex
                }
            );
            LOG(INFO) << "Presenting " << synchronizer.getInnerIndex();
            _renderer->getDevice().getPresent()->presentKHR(
                vk::PresentInfoKHR(
                    imageReady,
                    swapchain,
                    imageIndices
                )
            );
            LOG(INFO) << "Presented " << synchronizer.getInnerIndex();
            synchronizer.unlock();
        }

        void renderThread() {
            const vk::Device& device = _renderer->getVirtualDevice();
            un::SwapChain& swapChain = _renderer->getSwapChain();
            while (_rendering) {
                {
                    un::Chronometer chronometer;
                    un::SwapChain::ChainSynchronizer& synchronizer = swapChain.acquireSynchronizer();
                    synchronizer.access();
                    vk::Fence fence = synchronizer.getFence();
                    std::array<vk::Fence, 1> fencesToWait(
                        {fence}
                    );
                    if (device.waitForFences(
                        fencesToWait,
                        true,
                        1000
                    ) != vk::Result::eSuccess) {
                        continue;
                    }
                    device.resetFences(
                        fencesToWait
                    );
                    auto result = device.acquireNextImageKHR(
                        swapChain.getSwapChain(),
                        1000,
                        synchronizer.getImageReady()
                    );
                    if (result.result != vk::Result::eSuccess) {
                        continue;
                    }
                    u32 value = result.value;
                    u32 imageIndex = value;
                    auto graph = _renderer->getRenderGraph();
                    un::FrameData& data = _inFlightFrames[imageIndex];
                    data.renderPass = graph.getVulkanPass();
                    data.passesOutputs.resize(graph.getSize());
                    data.framebuffer = graph.getFrameBuffer(imageIndex);
                    schedulePasses(
                        imageIndex,
                        data,
                        _jobSystem,
                        synchronizer
                    );
                    size_t msTime = chronometer.stop();
                    _frame++;
                }
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
            _frame(0),
            _thread(nullptr) {
            _inFlightFrames.resize(renderer->getSwapChain().getNumLinks());
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