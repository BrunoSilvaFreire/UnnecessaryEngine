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
    class render_thread : public un::app_extension {
    private:
        bool _rendering = false;
        std::size_t _frame;
        un::steady_loop _loop;
        JobSystemType* _jobSystem;
        un::renderer* _renderer;
        un::thread* _thread;
        std::vector<un::frame_data> _inFlightFrames;

        void schedule_passes(
            u32 framebufferIndex,
            un::frame_data& frameData,
            JobSystemType* jobSystem,
            un::swap_chain::chain_synchronizer& synchronizer
        ) {
            const un::render_graph& graph = _renderer->get_render_graph();
            un::graphics_chain chain;
            std::unordered_map<u32, un::job_handle> vertexIndex2JobHandle;
            std::unordered_map<u32, std::set<u32>> passesDependencies;
            graph.each(
                [&](u32 index, const std::unique_ptr<un::render_pass>& pass) {
                    un::job_handle handle = chain.immediately<un::record_pass_job>(
                        &frameData,
                        pass.get(), //TODO: ONO http://open-std.org/JTC1/sc22/wg21/docs/papers/2014/n4282.pdf
                        index
                    );
                    std::string jobName = "Record Pass: \"";
                    jobName += pass->get_name();
                    jobName += "\" Frame: ";
                    jobName += std::to_string(_frame);
                    chain.set_name(handle, jobName);
                    vertexIndex2JobHandle[index] = handle;
                },
                [&](u32 from, u32 to, const un::dependency_type& type) {
                    passesDependencies[to].emplace(from);
                }
            );
            for (auto [passIndex, passDependencies] : passesDependencies) {
                un::job_handle passHandle = vertexIndex2JobHandle[passIndex];
                for (u32 dependencyIndex : passDependencies) {
                    un::job_handle dependencyHandle = vertexIndex2JobHandle[dependencyIndex];
                    chain.after(dependencyHandle, passHandle);
                }
            }
            std::ostringstream name;
            name << "Submit Render Commands (Frame: " << _frame
                 << ", FrameBuffer: " << framebufferIndex << ")";
            chain.finally(
                name.str(),
                [&, framebufferIndex]() {
                    submit_commands(
                        framebufferIndex,
                        frameData,
                        synchronizer,
                        graph,
                        _renderer->get_swap_chain().get_swap_chain()
                    );
                }
            );
            chain.submit(jobSystem);
        }

        void submit_commands(
            u32 framebufferIndex,
            const frame_data& frameData,
            swap_chain::chain_synchronizer& synchronizer,
            const render_graph& graph,
            vk::SwapchainKHR swapchain
        ) const {
            std::vector<vk::SubmitInfo> submits;
            graph.each(
                [&](u32 index, const std::unique_ptr<render_pass>& pass) {
                    const pass_output& passOutput = frameData.passesOutputs[index];

                    std::vector<u32> dependencies = graph
                        .dependencies_of(index)
                        .get_dependencies();
                    std::vector<u32> dependants = graph
                        .dependants_on(index)
                        .get_dependencies();
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
                [&](u32 from, u32 to, const dependency_type& type) {
                }
            );

            vk::Queue queue = *_renderer->get_device().get_graphics();
            queue.submit(
                submits,
                synchronizer.get_fence()
            );
            std::array<vk::Semaphore, 1> imageReady(
                {
                    synchronizer.get_image_ready()
                }
            );
            std::array<u32, 1> imageIndices(
                {
                    framebufferIndex
                }
            );
            VK_CALL(
                _renderer->get_device().get_present()->presentKHR(
                    vk::PresentInfoKHR(
                        imageReady,
                        swapchain,
                        imageIndices
                    )
                )
            );
            synchronizer.unlock();
        }

        void render_thread_aaa() {
            const vk::Device& device = _renderer->get_virtual_device();
            un::swap_chain& swapChain = _renderer->get_swap_chain();
            window* pWindow = _renderer->get_window();
            while (_rendering) {
                un::chronometer chronometer;
                _loop.enter();
                un::swap_chain::chain_synchronizer& synchronizer = swapChain.acquire_synchronizer();
                synchronizer.access();
                vk::Fence fence = synchronizer.get_fence();
                std::array<vk::Fence, 1> fencesToWait(
                    {fence}
                );
                std::chrono::nanoseconds timeout = _loop.get_loop_time_frame();
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
                    swapChain.get_swap_chain(),
                    timeout.count(),
                    synchronizer.get_image_ready()
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
                const auto& graph = _renderer->get_render_graph();
                un::frame_data& data = _inFlightFrames[imageIndex];
                data.renderPass = graph.get_vulkan_pass();
                data.passesOutputs.resize(graph.get_size());
                data.framebuffer = graph.get_frame_buffer(imageIndex);
                schedule_passes(
                    imageIndex,
                    data,
                    _jobSystem,
                    synchronizer
                );
                _loop.exit();
                _frame++;
                _rendering = !pWindow->should_close();
            }
        }

    public:
        void start() {
            if (_rendering) {
                return;
            }
            _loop.set_frequency(165);
            _rendering = true;
            _thread = new un::thread(
                "RenderThread",
                std::bind(&render_thread::render_thread_aaa, this)
            );
            _thread->start();
        }

        void stop() {
            _rendering = false;
            _thread->join();
            delete _thread;
        }

        render_thread(
            JobSystemType* jobSystem,
            renderer* renderer
        ) : _jobSystem(jobSystem),
            _renderer(renderer),
            _frame(0),
            _thread(nullptr) {
            _inFlightFrames.resize(renderer->get_swap_chain().get_num_links());
        }

        void apply(application& application) override {
            application.get_on_start() += [this]() {
                start();
            };
            application.get_on_stop() += [this]() {
                stop();
            };
        }
    };
}
#endif
