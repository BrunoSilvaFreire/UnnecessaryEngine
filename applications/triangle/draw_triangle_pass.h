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
    class draw_triangle_pass : public render_pass {
    public:
        draw_triangle_pass(
            const vk::Rect2D& renderArea,
            std::vector<vk::ClearValue> clears,
            std::size_t colorAttachmentIndex,
            std::size_t depthAttachmentIndex
        ) : render_pass(
            "Draw Triangle",
            vk::PipelineStageFlagBits::eAllGraphics
        ),
            _renderArea(renderArea),
            _clears(std::move(clears)),
            _trianglePipeline() {
            uses_color_attachment(colorAttachmentIndex, vk::ImageLayout::eColorAttachmentOptimal);
            uses_depth_attachment(depthAttachmentIndex, vk::ImageLayout::eDepthStencilAttachmentOptimal);
        }

    private:
        vk::Rect2D _renderArea;
        std::vector<vk::ClearValue> _clears;
        std::optional<pipeline> _trianglePipeline;

    public:
        void on_vulkan_pass_created(vk::RenderPass renderPass, renderer& renderer) override {
            _trianglePipeline = un::pipelines::createTrianglePipeline(
                renderer.get_virtual_device(),
                renderPass
            );
        }

        void record(
            const frame_data& data,
            command_buffer& cmdBuffer
        ) const override {
            if (!_trianglePipeline.has_value()) {
                return;
            }
            cmdBuffer->beginRenderPass(
                vk::RenderPassBeginInfo(
                    data.renderPass,
                    data.framebuffer,
                    _renderArea,
                    _clears
                ),
                vk::SubpassContents::eInline
            );
            std::vector<vk::DescriptorSet> pipelineDescriptorSets;
            std::array<uint32_t, 0> dynamicOffsets{};
            if (!pipelineDescriptorSets.empty()) {
                cmdBuffer->bindDescriptorSets(
                    vk::PipelineBindPoint::eGraphics,
                    _trianglePipeline->get_layout(),
                    0,
                    pipelineDescriptorSets,
                    dynamicOffsets
                );
            }

            cmdBuffer->bindPipeline(
                vk::PipelineBindPoint::eGraphics,
                _trianglePipeline->operator*()
            );
            cmdBuffer->setViewport(
                0,
                std::vector<vk::Viewport>(
                    {
                        vk::Viewport(
                            0, 0,
                            _renderArea.extent.width,
                            _renderArea.extent.height,
                            0,
                            1.0F
                        )
                    })
            );
            cmdBuffer->setScissor(
                0,
                std::vector<vk::Rect2D>(
                    {
                        _renderArea
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
