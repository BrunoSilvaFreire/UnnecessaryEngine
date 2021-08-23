#include <unnecessary/systems/graphics_systems.h>

namespace un {
    PrepareFrameGraphSystem::PrepareFrameGraphSystem(
        Renderer* renderer
    ) : renderer(renderer) {}

    u32 PrepareFrameGraphSystem::enqueuePreparationPhase(
        const vk::CommandBuffer& buffer,
        vk::PipelineStageFlagBits bits
    ) {
        auto id = commandGraph.enqueueCommandBuffer(buffer, bits);
        preparationCommands.emplace(id);
        return id;
    }

    void PrepareFrameGraphSystem::step(World& world, f32 delta, un::JobWorker* worker) {
        u32 framebufferIndex;
        auto currentFramebuffer = nextFramebuffer(&framebufferIndex);
        const vk::Device& device = renderer->getVirtualDevice();
        const vk::SwapchainKHR& chain = renderer->getSwapChain().getSwapChain();
        vkCall(
            device.acquireNextImageKHR(
                chain,
                std::numeric_limits<u64>::max(),
                imageAvailableSemaphore,
                nullptr,
                &framebufferIndex
            )
        );
        commandGraph.clear();
        renderer->getCurrentPipeline()->begin(currentFramebuffer);
    }

    CommandBufferGraph& PrepareFrameGraphSystem::getCommandGraph() {
        return commandGraph;
    }

    vk::Framebuffer PrepareFrameGraphSystem::getCurrentFramebuffer() {
        return framebuffers[currentFramebufferIndex];
    }

    void PrepareFrameGraphSystem::describe(SystemDescriptor& descriptor) {
        renderingPipeline = descriptor.renderer<un::DummyRenderingPipeline>(renderer);
        if (renderingPipeline != nullptr) {
            auto& chain = renderer->getSwapChain();
            auto device = renderer->getVirtualDevice();
            const un::Size2D& resolution = chain.getResolution();
            std::vector<vk::ImageView> views;
            for (const auto& item : chain.getViews()) {
                auto view = item.getVulkanView();
                framebuffers.emplace_back(
                    device.createFramebuffer(
                        vk::FramebufferCreateInfo(
                            (vk::FramebufferCreateFlags) 0,
                            renderingPipeline->getBakedGraph()->getRenderPass(),
                            1, &view,
                            resolution.x,
                            resolution.y,
                            1
                        )
                    )
                );
            }
            imageAvailableSemaphore = device.createSemaphore(
                vk::SemaphoreCreateInfo(
                    (vk::SemaphoreCreateFlags) 0
                )
            );
        }
    }

    void DispatchFrameGraphSystem::step(World& world, f32 delta, un::JobWorker* worker) {

        un::Size2D size = renderer->getSwapChain().getResolution();
        const vk::Rect2D& renderArea = vk::Rect2D(
            vk::Offset2D(0, 0),
            vk::Extent2D(size.x, size.y)
        );
        un::CommandBuffer primaryBuffer(
            *renderer,
            worker->getGraphicsResources().getCommandPool()
        );
        std::array<float, 4> colorArr{};
        colorArr[0] = clearColor.r;
        colorArr[1] = clearColor.g;
        colorArr[2] = clearColor.b;
        colorArr[3] = clearColor.a;

        vk::ClearColorValue clearColorValue;
        vk::ClearValue clear(clearColorValue);
        clearColorValue.setFloat32(colorArr);
        un::BakedFrameGraph* bakedGraph = renderingPipeline->getBakedGraph();
        primaryBuffer->begin(
            vk::CommandBufferBeginInfo(
                vk::CommandBufferUsageFlagBits::eOneTimeSubmit |
                vk::CommandBufferUsageFlagBits::eRenderPassContinue
            )
        );
        primaryBuffer->beginRenderPass(
            vk::RenderPassBeginInfo(
                bakedGraph->getRenderPass(),
                graphSystem->getCurrentFramebuffer(),
                renderArea,
                1,
                &clear
            ),
            vk::SubpassContents::eInline
        );
        std::vector<vk::CommandBuffer> buffers;
        for (auto[first, second] : bakedGraph->getFrameGraph().all_vertices()) {
            buffers.emplace_back(first->getCommandBuffer());
        }
        primaryBuffer->executeCommands(buffers);
        primaryBuffer->endRenderPass();
        primaryBuffer->end();
        auto commandGraph = graphSystem->getCommandGraph();
        auto renderSceneCommand = commandGraph.enqueueCommandBuffer(
            *primaryBuffer,
            vk::PipelineStageFlagBits::eBottomOfPipe
        );
        for (u32 preparationCommand : graphSystem->preparationCommands) {
            commandGraph.addDependency(renderSceneCommand, preparationCommand);
        }
        un::Queue& graphics = renderer->getGraphics();
        auto chain = renderer->getSwapChain().getSwapChain();
        commandGraph.submit(renderer->getVirtualDevice(), graphics);
        u32 currentFramebuffer = graphSystem->currentFramebufferIndex;
        vkCall(
            graphics->presentKHR(
                vk::PresentInfoKHR(
                    1, &graphSystem->imageAvailableSemaphore,
                    1, &chain,
                    &currentFramebuffer,
                    nullptr
                )
            )
        );
    }

    void DispatchFrameGraphSystem::describe(SystemDescriptor& descriptor) {
        un::System::describe(descriptor);
        graphSystem = descriptor.dependsOn<un::PrepareFrameGraphSystem>();
        renderingPipeline = descriptor.renderer<un::DummyRenderingPipeline>(renderer);
    }

    DispatchFrameGraphSystem::DispatchFrameGraphSystem(
        Renderer* renderer
    ) : renderer(renderer),
        clearColor(0.1, 0.1, 0.1, 1) {

    }

    vk::Framebuffer PrepareFrameGraphSystem::nextFramebuffer(u32* framebufferIndexResult) {
        *framebufferIndexResult = currentFramebufferIndex;
        auto buffer = framebuffers[currentFramebufferIndex++];
        currentFramebufferIndex %= framebuffers.size();
        return buffer;
    }
}