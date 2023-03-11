//
// Created by bruno on 21/12/2021.
//

#ifndef UNNECESSARYENGINE_DRAW_OBJECTS_PASS_H
#define UNNECESSARYENGINE_DRAW_OBJECTS_PASS_H

#include <unnecessary/rendering/render_pass.h>
#include <unnecessary/rendering/render_group.h>
#include <unnecessary/rendering/render_graph.h>
#include <unnecessary/rendering/buffers/command_buffer.h>

namespace un {

    class draw_objects_pass : public un::render_pass {
    public:
        draw_objects_pass(
            render_group* objects,
            const vk::Rect2D& renderArea,
            const std::vector<vk::ClearValue>& clears,
            std::size_t colorAttachmentIndex,
            std::size_t depthAttachmentIndex
        ) : render_pass(
            "Draw Objects",
            vk::PipelineStageFlagBits::eAllGraphics
        ),
            _objects(objects),
            _renderArea(renderArea),
            _clears(clears) {
            uses_color_attachment(colorAttachmentIndex, vk::ImageLayout::eColorAttachmentOptimal);
            uses_depth_attachment(
                depthAttachmentIndex,
                vk::ImageLayout::eDepthStencilAttachmentOptimal
            );
        }

    private:
        un::render_group* _objects;
        vk::Rect2D _renderArea;
        std::vector<vk::ClearValue> _clears;
    public:
        void record(
            const un::frame_data& data,
            un::command_buffer& cmdBuffer
        ) const override {
            // This is pre-ordered by render group
            cmdBuffer->beginRenderPass(
                vk::RenderPassBeginInfo(
                    data.renderPass,
                    data.framebuffer,
                    _renderArea,
                    _clears
                ),
                vk::SubpassContents::eSecondaryCommandBuffers
            );
            for (const un::material_batch& materialBatch : _objects->get_material_batches()) {
                const un::material* pMaterial = materialBatch.get_material();
                const un::pipeline* pPipeline = pMaterial->get_pipeline();
                std::vector<vk::DescriptorSet> pipelineDescriptorSets;
                std::array<uint32_t, 0> dynamicOffsets{};
                if (!pipelineDescriptorSets.empty()) {
                    cmdBuffer->bindDescriptorSets(
                        vk::PipelineBindPoint::eGraphics,
                        pPipeline->get_layout(),
                        0,
                        pipelineDescriptorSets,
                        dynamicOffsets
                    );
                }

                cmdBuffer->bindPipeline(
                    vk::PipelineBindPoint::eGraphics,
                    pPipeline->operator*()
                );

                for (const auto& [geometry, batch] : materialBatch.get_geometry_batches()) {
                    std::array<vk::Buffer, 1> vertexBufs = {
                        *geometry->get_vertex_buffer()
                    };
                    std::array<const vk::DeviceSize, 1> vertexOffset = {
                        0
                    };

                    cmdBuffer->bindVertexBuffers(
                        0,
                        vertexBufs,
                        vertexOffset
                    );
                    cmdBuffer->bindIndexBuffer(
                        *geometry->get_index_buffer(),
                        0,
                        vk::IndexType::eUint16
                    );
                    const auto& drawables = batch.get_drawables();
                    cmdBuffer->drawIndexed(
                        geometry->get_num_vertices(),
                        drawables.size(),
                        0,
                        0,
                        0
                    );
                }
            }
            cmdBuffer->endRenderPass();
        }

    };
}
#endif