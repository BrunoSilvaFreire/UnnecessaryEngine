#include <unnecessary/graphics/renderer.h>
#include <unnecessary/graphics/rendering/baked_frame_graph.h>

namespace un {

    un::RenderPassDescriptor FrameGraph::addPass(
        const std::string& passName,
        vk::PipelineStageFlags flags
    ) {
        un::RenderPassDescription pass(passName, flags);
        u32 index = push(std::move(pass));
        independent.emplace(index);
        return {index, this};
    }

    un::RenderPassDescriptor FrameGraph::addPass(
        const std::string& passName,
        vk::PipelineStageFlags flags,
        un::RenderPassDescription** result
    ) {
        auto descriptor = addPass(passName, flags);
        *result = descriptor.getPass();
        return descriptor;
    }

    un::FrameResourceReference FrameGraph::addResource(
        const std::string& resourceName
    ) {
        size_t index = resources.size();
        auto& reference = resources.emplace_back(resourceName);
        return un::FrameResourceReference(index, &reference);
    }

    const std::vector<un::Attachment>& FrameGraph::getAttachments() const {
        return attachments;
    }

    const std::set<u32>& FrameGraph::getBorrowedAttachments() const {
        return borrowedAttachments;
    }

    bool FrameGraph::isAttachmentBorrowed(std::size_t index) const {
        return borrowedAttachments.contains(index);
    }

    BakedFrameGraph::BakedFrameGraph(
        un::FrameGraph&& graph,
        un::Renderer& renderer
    ) : frameGraph(std::move(graph)),
        frameBuffers() {
        std::vector<vk::SubpassDescription> subPasses;
        std::vector<vk::SubpassDependency> subPassesDependencies;
        subPasses.reserve(frameGraph.size());
        for (auto[vertex, index]: frameGraph.all_vertices()) {
            bakedPasses.emplace_back(&renderer);
            subPasses.emplace_back(
                (vk::SubpassDescriptionFlags) 0,
                vk::PipelineBindPoint::eGraphics,
                vertex->getUsedAttachments(),
                vertex->getColorAttachments(),
                vertex->getResolveAttachments(),
                vertex->getDepthAttachments().data()
            );
            for (auto[neighborIndex, edge]: frameGraph.edges_from(index)) {
                if (edge.getType() != un::RenderPassDependency::Type::eUses) {
                    continue;
                }
                auto neighbor = frameGraph.vertex(neighborIndex);
                subPassesDependencies.emplace_back(
                    index,
                    neighborIndex,
                    vertex->getStageFlags(),
                    neighbor->getStageFlags(),
                    edge.getSrcFlags(),
                    edge.getDstFlags(),
                    edge.getDependencyFlags()
                );
            }
        }
        std::vector<vk::AttachmentDescription> attachments;
        for (const auto& item: frameGraph.attachments) {
            attachments.emplace_back(item.getDescription());
        }

        auto device = renderer.getVirtualDevice();
        renderPass = device.createRenderPass(
            vk::RenderPassCreateInfo(
                (vk::RenderPassCreateFlags) 0,
                attachments,
                subPasses,
                subPassesDependencies
            )
        );
        auto& chain = renderer.getSwapChain();
        const un::Size2D& resolution = chain.getResolution();
        std::vector<vk::ImageView> views;
        for (int j = 0; j < chain.getViews().size(); ++j) {
            frameBuffers.emplace_back(
                &renderer,
                frameGraph,
                renderPass,
                j
            );
        }
    }


    const vk::RenderPass& BakedFrameGraph::getVulkanPass() const {
        return renderPass;
    }

    const FrameGraph& BakedFrameGraph::getFrameGraph() const {
        return frameGraph;
    }

    un::FrameGraph& BakedFrameGraph::getFrameGraph() {
        return frameGraph;
    }

    un::RenderPass& BakedFrameGraph::getRenderPass(u32 passIndex) {
        return bakedPasses[passIndex];
    }

    un::RenderPass& BakedFrameGraph::getRenderPass(
        const RenderPassDescriptor& descriptor
    ) {
        return getRenderPass(descriptor.getPassIndex());
    }

    un::FrameBuffer& BakedFrameGraph::getFrameBuffer(u32 i) {
        return frameBuffers[i];
    }
}