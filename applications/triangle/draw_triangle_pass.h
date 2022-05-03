//
// Created by bruno on 21/12/2021.
//

#ifndef UNNECESSARYENGINE_DRAW_TRIANGLE_PASS_H
#define UNNECESSARYENGINE_DRAW_TRIANGLE_PASS_H

#include <unnecessary/rendering/render_pass.h>
#include <unnecessary/rendering/render_group.h>
#include <unnecessary/rendering/render_graph.h>
#include <unnecessary/rendering/renderer.h>
#include <unnecessary/rendering/buffers/command_buffer.h>
#include <triangle.gen.h>

#include <utility>

namespace un {

    class DrawTrianglePass : public un::RenderPass {
    public:
        DrawTrianglePass(
            const vk::Rect2D& renderArea,
            std::vector<vk::ClearValue> clears,
            std::size_t colorAttachmentIndex,
            std::size_t depthAttachmentIndex
        ) : RenderPass(
            "Draw Triangle",
            vk::PipelineStageFlagBits::eAllGraphics
        ),
            renderArea(renderArea),
            clears(std::move(clears)),
            trianglePipeline() {
            usesColorAttachment(colorAttachmentIndex, vk::ImageLayout::eColorAttachmentOptimal);
            usesDepthAttachment(depthAttachmentIndex, vk::ImageLayout::eDepthStencilAttachmentOptimal);
        }

    private:
        vk::Rect2D renderArea;
        std::vector<vk::ClearValue> clears;
        std::optional<un::Pipeline> trianglePipeline;
    public:
        void onVulkanPassCreated(vk::RenderPass renderPass, un::Renderer& renderer) override {
            trianglePipeline = un::pipelines::createTrianglePipeline(renderer.getVirtualDevice(), renderPass);
        }

        void record(
            const un::FrameData& data,
            un::CommandBuffer& cmdBuffer
        ) const override {
            if (!trianglePipeline.has_value()) {
                return;
            }
            cmdBuffer->beginRenderPass(
                vk::RenderPassBeginInfo(
                    data.renderPass,
                    data.framebuffer,
                    renderArea,
                    clears
                ),
                vk::SubpassContents::eInline
            );
            std::vector<vk::DescriptorSet> pipelineDescriptorSets;
            std::array<uint32_t, 0> dynamicOffsets{};
            if (!pipelineDescriptorSets.empty()) {
                cmdBuffer->bindDescriptorSets(
                    vk::PipelineBindPoint::eGraphics,
                    trianglePipeline->getLayout(),
                    0,
                    pipelineDescriptorSets,
                    dynamicOffsets
                );
            }

            cmdBuffer->bindPipeline(
                vk::PipelineBindPoint::eGraphics,
                trianglePipeline->operator*()
            );
            cmdBuffer->setViewport(
                0,
                std::vector<vk::Viewport>(
                    {
                        vk::Viewport(
                            0, 0,
                            renderArea.extent.width,
                            renderArea.extent.height,
                            0,
                            1.0F
                        )
                    })
            );
            cmdBuffer->setScissor(
                0,
                std::vector<vk::Rect2D>(
                    {
                        renderArea
                    }
                )
            );
            cmdBuffer->draw(
                3,
                1,
                0,
                0
            );
            cmdBuffer->endRenderPass();
        }

    };
}
#endif