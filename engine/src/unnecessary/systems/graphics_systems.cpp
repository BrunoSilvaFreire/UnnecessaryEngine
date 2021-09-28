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
        preparationCommands.clear();
        const vk::Device& device = renderer->getVirtualDevice();
        const vk::SwapchainKHR& chain = renderer->getSwapChain().getSwapChain();
        vkCall(
            device.acquireNextImageKHR(
                chain,
                std::numeric_limits<u64>::max(),
                imageAvailableSemaphore,
                nullptr,
                &currentFramebufferIndex
            )
        );
        auto currentFramebuffer = framebuffers[currentFramebufferIndex];
        commandGraph.clear();
        renderingPipeline->begin(renderer, currentFramebuffer);
    }

    CommandBufferGraph& PrepareFrameGraphSystem::getCommandGraph() {
        return commandGraph;
    }

    vk::Framebuffer PrepareFrameGraphSystem::getCurrentFramebuffer() {
        return framebuffers[currentFramebufferIndex];
    }

    void PrepareFrameGraphSystem::describe(SystemDescriptor& descriptor) {
        descriptor.runsOnStage(un::kUploadFrameData);
        renderingPipeline = descriptor.usesPipeline<un::DummyRenderingPipeline>(renderer);
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
                            renderingPipeline->unsafeGetFrameGraph().getVulkanPass(),
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

        std::vector<vk::ClearValue> clears;
        un::BakedFrameGraph& bakedGraph = renderingPipeline->unsafeGetFrameGraph();
        for (const auto& item : bakedGraph.getFrameGraph().getAttachments()) {
            clears.emplace_back(item.getClear());
        }
        primaryBuffer->begin(
            vk::CommandBufferBeginInfo(
                vk::CommandBufferUsageFlagBits::eOneTimeSubmit |
                vk::CommandBufferUsageFlagBits::eRenderPassContinue
            )
        );
        u32 framebufferIndex;
        primaryBuffer->beginRenderPass(
            vk::RenderPassBeginInfo(
                bakedGraph.getVulkanPass(),
                graphSystem->getCurrentFramebuffer(&framebufferIndex),
                renderArea,
                clears
            ),
            vk::SubpassContents::eSecondaryCommandBuffers
        );
        std::vector<vk::CommandBuffer> buffers;
        for (auto[first, second] : bakedGraph.getFrameGraph().all_vertices()) {

            un::RenderPass& pass = bakedGraph.getRenderPass(second);
            pass.end();
            buffers.emplace_back(pass.getCommandBuffer());
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
        vkCall(
            graphics->presentKHR(
                vk::PresentInfoKHR(
                    1, &graphSystem->imageAvailableSemaphore,
                    1, &chain,
                    &framebufferIndex,
                    nullptr
                )
            )
        );
    }

    void DispatchFrameGraphSystem::describe(SystemDescriptor& descriptor) {
        descriptor.runsOnStage(un::kDispatchFrame);
        graphSystem = descriptor.dependsOn<un::PrepareFrameGraphSystem>();
        renderingPipeline = descriptor.usesPipeline<un::DummyRenderingPipeline>(renderer);
    }

    DispatchFrameGraphSystem::DispatchFrameGraphSystem(
        Renderer* renderer
    ) : renderer(renderer) {

    }

    vk::Framebuffer PrepareFrameGraphSystem::getCurrentFramebuffer(u32* pIndex) {
        *pIndex = currentFramebufferIndex;
        return framebuffers[currentFramebufferIndex];
    }
}