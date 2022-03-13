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

    class DrawObjectsPass : public un::RenderPass {
    public:
        DrawObjectsPass(
            RenderGroup* objects,
            const vk::Rect2D& renderArea,
            const std::vector<vk::ClearValue>& clears,
            std::size_t colorAttachmentIndex,
            std::size_t depthAttachmentIndex
        ) : RenderPass(
            "Draw Objects",
            vk::PipelineStageFlagBits::eAllGraphics
        ),
            objects(objects),
            renderArea(renderArea),
            clears(clears) {
            usesColorAttachment(colorAttachmentIndex, vk::ImageLayout::eColorAttachmentOptimal);
            usesDepthAttachment(depthAttachmentIndex, vk::ImageLayout::eDepthAttachmentOptimal);
        }

    private:
        un::RenderGroup* objects;
        vk::Rect2D renderArea;
        std::vector<vk::ClearValue> clears;
    public:
        void record(
            const un::FrameData& data,
            un::CommandBuffer& cmdBuffer
        ) const override {
            // This is pre-ordered by render group
            cmdBuffer->beginRenderPass(
                vk::RenderPassBeginInfo(
                    data.renderPass,
                    data.framebuffer,
                    renderArea,
                    clears
                ),
                vk::SubpassContents::eSecondaryCommandBuffers
            );
            for (const un::MaterialBatch& materialBatch : objects->getMaterialBatches()) {
                const un::Material* pMaterial = materialBatch.getMaterial();
                const un::Pipeline* pPipeline = pMaterial->getPipeline();
                std::vector<vk::DescriptorSet> pipelineDescriptorSets;
                std::array<uint32_t, 0> dynamicOffsets{};
                cmdBuffer->bindDescriptorSets(
                    vk::PipelineBindPoint::eGraphics,
                    pPipeline->getLayout(),
                    0,
                    pipelineDescriptorSets,
                    dynamicOffsets
                );
                cmdBuffer->bindPipeline(
                    vk::PipelineBindPoint::eGraphics,
                    pPipeline->operator*()
                );

                for (const auto &[geometry, batch] : materialBatch.getGeometryBatches()) {
                    std::array<vk::Buffer, 1> vertexBufs = {
                        *geometry->getVertexBuffer()
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
                        *geometry->getIndexBuffer(),
                        0,
                        vk::IndexType::eUint16
                    );
                    const auto& drawables = batch.getDrawables();
                    cmdBuffer->drawIndexed(
                        geometry->getNumVertices(),
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