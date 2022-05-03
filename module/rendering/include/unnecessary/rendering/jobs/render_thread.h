#ifndef UNNECESSARYENGINE_RENDER_THREAD_H
#define UNNECESSARYENGINE_RENDER_THREAD_H

#include <unnecessary/misc/benchmark.h>
#include <unnecessary/application/loop.h>
#include <unnecessary/application/application.h>
#include <unnecessary/rendering/renderer.h>
#include <unnecessary/rendering/jobs/render_job.h>
#include <grapphs/algorithms/rlo_traversal.h>

namespace un {

    template<typename JobSystemType>
    class RenderThread : public un::Extension {
    private:
        bool _rendering = false;
        std::size_t _frame;
        un::SteadyLoop _loop;
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
            const un::RenderGraph& graph = _renderer->getRenderGraph();
            un::GraphicsChain chain;
            std::unordered_map<u32, un::JobHandle> vertexIndex2JobHandle;
            std::unordered_map<u32, std::set<u32>> passesDependencies;
            graph.each(
                [&](u32 index, const std::unique_ptr<un::RenderPass>& pass) {
                    un::JobHandle handle = chain.immediately<un::RecordPassJob>(
                        &frameData,
                        pass.get(), //TODO: ONO http://open-std.org/JTC1/sc22/wg21/docs/papers/2014/n4282.pdf
                        index
                    );
                    std::string jobName = "Record Pass: \"";
                    jobName += pass->getName();
                    jobName += "\" Frame: ";
                    jobName += std::to_string(_frame);
                    chain.setName(handle, jobName);
                    vertexIndex2JobHandle[index] = handle;
                },
                [&](u32 from, u32 to, const un::DependencyType& type) {
                    passesDependencies[to].emplace(from);
                }
            );
            for (auto [passIndex, passDependencies] : passesDependencies) {
                un::JobHandle passHandle = vertexIndex2JobHandle[passIndex];
                for (u32 dependencyIndex : passDependencies) {
                    un::JobHandle dependencyHandle = vertexIndex2JobHandle[dependencyIndex];
                    chain.after(dependencyHandle, passHandle);
                }
            }
            std::ostringstream name;
            name << "Submit Render Commands (Frame: " << _frame
                 << ", FrameBuffer: " << framebufferIndex << ")";
            chain.finally(
                name.str(),
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
            std::vector<vk::SubmitInfo> submits;
            graph.each(
                [&](u32 index, const std::unique_ptr<RenderPass>& pass) {
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
                [&](u32 from, u32 to, const DependencyType& type) { }
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
            vkCall(
                _renderer->getDevice().getPresent()->presentKHR(
                    vk::PresentInfoKHR(
                        imageReady,
                        swapchain,
                        imageIndices
                    )
                )
            );
            synchronizer.unlock();
        }

        void renderThread() {
            const vk::Device& device = _renderer->getVirtualDevice();
            un::SwapChain& swapChain = _renderer->getSwapChain();
            Window* pWindow = _renderer->getWindow();
            while (_rendering) {
                un::Chronometer chronometer;
                _loop.enter();
                un::SwapChain::ChainSynchronizer& synchronizer = swapChain.acquireSynchronizer();
                synchronizer.access();
                vk::Fence fence = synchronizer.getFence();
                std::array<vk::Fence, 1> fencesToWait(
                    {fence}
                );
                std::chrono::nanoseconds timeout = _loop.getLoopTimeFrame();
                vk::Result waitForFences = device.waitForFences(
                    fencesToWait,
                    true,
                    timeout.count()
                );
                if (waitForFences != vk::Result::eSuccess) {
                    LOG(WARN)
                        << "Unable to wait for fence ("
                        << vk::to_string(waitForFences)
                        << ")";
                    synchronizer.unlock();
                    continue;
                }
                device.resetFences(
                    fencesToWait
                );

                auto result = device.acquireNextImageKHR(
                    swapChain.getSwapChain(),
                    timeout.count(),
                    synchronizer.getImageReady()
                );
                if (result.result != vk::Result::eSuccess) {
                    LOG(WARN) << "Unable to acquire image ("
                              << vk::to_string(result.result)
                              << ");";
                    synchronizer.unlock();
                    continue;
                }
                u32 value = result.value;
                u32 imageIndex = value;
                const auto& graph = _renderer->getRenderGraph();
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
                _loop.exit();
                _frame++;
                _rendering = !pWindow->shouldClose();
            }
        }

    public:
        void start() {
            if (_rendering) {
                return;
            }
            _loop.setFrequency(165);
            _rendering = true;
            _thread = new un::Thread(
                "RenderThread",
                std::bind(&RenderThread::renderThread, this)
            );
            _thread->start();
        }

        void stop() {
            _rendering = false;
            _thread->join();
            delete _thread;
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
