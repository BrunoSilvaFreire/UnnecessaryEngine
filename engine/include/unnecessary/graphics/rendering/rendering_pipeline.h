#ifndef UNNECESSARYENGINE_RENDERING_PIPELINE_H
#define UNNECESSARYENGINE_RENDERING_PIPELINE_H

#include <unnecessary/graphics/frame_graph.h>
#include <unnecessary/jobs/jobs.h>

namespace un {
    class RenderingPipeline {
    protected:
        un::BakedFrameGraph* frameGraph;
        Renderer* renderer;

        virtual void configureInheritance(
            vk::CommandBufferInheritanceInfo& inheritance
        ) {}

    public:
        RenderingPipeline(un::Renderer* renderer) : frameGraph(nullptr),
                                                    renderer(renderer) {

        }

        virtual ~RenderingPipeline() {
            delete frameGraph;
        };

        void begin(vk::Framebuffer framebuffer);

        BakedFrameGraph* getFrameGraph() const;
    };


    class DummyRenderingPipeline : public un::RenderingPipeline {
    private:
        un::RenderPass* drawOpaquePass{};
        un::RenderPass* depthPass{};
        un::FrameResource* depthBuffer{};
        std::size_t colorAttachmentHandle{};

    public:
        DummyRenderingPipeline(un::Renderer* renderer) : un::RenderingPipeline(renderer) {
            un::FrameGraph graph;
            colorAttachmentHandle = graph.addAttachment(
                vk::ClearColorValue(
                    std::array<float, 4>(
                        {
                            0.1F, 0.1F, 0.1F, 1.0F
                        }
                    )
                ),
                (vk::AttachmentDescriptionFlags) 0,
                vk::Format::eB8G8R8A8Unorm,
                vk::SampleCountFlagBits::e1,
                vk::AttachmentLoadOp::eClear,
                vk::AttachmentStoreOp::eStore,
                vk::AttachmentLoadOp::eDontCare,
                vk::AttachmentStoreOp::eDontCare,
                vk::ImageLayout::eUndefined,
                vk::ImageLayout::ePresentSrcKHR
            );
            auto drawOpaque = graph.addPass(
                *renderer,
                "drawOpaquePass",
                vk::PipelineStageFlagBits::eFragmentShader |
                vk::PipelineStageFlagBits::eVertexShader |
                vk::PipelineStageFlagBits::eComputeShader,
                &drawOpaquePass
            );
            drawOpaque.usesColorAttachment(
                colorAttachmentHandle,
                vk::ImageLayout::eColorAttachmentOptimal
            );
            frameGraph = new un::BakedFrameGraph(std::move(graph), *renderer);
        }

        un::RenderPass* getDepthPass() const {
            return depthPass;
        }

        un::RenderPass* getDrawOpaquePass() const {
            return drawOpaquePass;
        }

        un::FrameResource* getDepthBuffer() const {
            return depthBuffer;
        }

        BakedFrameGraph* getBakedGraph() const {
            return frameGraph;
        }

        vk::RenderPass getRenderPass() const {
            return frameGraph->getRenderPass();
        }

    protected:

        void configureInheritance(
            vk::CommandBufferInheritanceInfo& inheritance
        ) override {
            inheritance.setRenderPass(frameGraph->getRenderPass());
        }
    };
}
#endif