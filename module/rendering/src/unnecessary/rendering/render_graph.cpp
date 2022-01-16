#include <unnecessary/rendering/render_graph.h>
#include <unnecessary/rendering/render_pass.h>

namespace un {
    void RenderGraph::bake(vk::Device device) {
        std::vector<vk::SubpassDescription> subPasses;
        std::vector<vk::SubpassDependency> subPassesDependencies;
        subPasses.reserve(getSize());
        for (std::size_t i = 0; i < getSize(); ++i) {
            un::RenderPass* vertex = *(this->operator[](i));
            subPasses.emplace_back(
                (vk::SubpassDescriptionFlags) 0,
                vk::PipelineBindPoint::eGraphics,
                vertex->getUsedAttachments(),
                vertex->getColorAttachments(),
                vertex->getResolveAttachments(),
                vertex->getDepthAttachments().data()
            );
            for (auto[neighborIndex, edge] : dependenciesOf(i)) {
                auto neighbor = *this->operator[](neighborIndex);
                // TODO: Compute these
                vk::AccessFlags srcFlags;
                vk::AccessFlags dstFlags;
                vk::DependencyFlags dependencyFlags;
                subPassesDependencies.emplace_back(
                    i,
                    neighborIndex,
                    vertex->getStageFlags(),
                    neighbor->getStageFlags(),
                    srcFlags,
                    dstFlags,
                    dependencyFlags
                );
            }
        }
        std::vector<vk::AttachmentDescription> attachments;
        /*for (const auto& item: frameGraph.attachments) {
            attachments.emplace_back(item.getDescription());
        }*/

        renderPass = device.createRenderPass(
            vk::RenderPassCreateInfo(
                (vk::RenderPassCreateFlags) 0,
                attachments,
                subPasses,
                subPassesDependencies
            )
        );
    }
}