#include <unnecessary/graphics/frame_graph.h>

#include <utility>
#include <unnecessary/logging.h>
#include <unnecessary/graphics/renderer.h>
#include <unnecessary/misc/aggregator.h>
#include <unnecessary/graphics/rendering/rendering_pipeline.h>

namespace un {

    void RenderPassDescriptor::writesTo(const FrameResourceReference& reference) {
        un::FrameResource* resource;
        if (!tryFindResource(reference, &resource)) {
            un::RenderPass* pPass = frameGraph->vertex(passIndex);
            LOG(FUCK) << "Render pass '" << GREEN(pPass->getName())
                      << "' tried to write to resource " <<
                      reference.getIndex()
                      << ", which is not found in the frame graph it lives in.";
            return;
        }
        resource->writePasses.emplace(passIndex);
        for (u32 pass : resource->readPasses) {
            frameGraph->independent.erase(pass);
        }
    }

    void RenderPassDescriptor::readsFrom(const FrameResourceReference& reference) {
        un::FrameResource* resource;
        if (!tryFindResource(reference, &resource)) {
            un::RenderPass* pPass = frameGraph->vertex(passIndex);
            LOG(FUCK) << "Render pass '" << GREEN(pPass->getName())
                      << "' tried to read from resource " <<
                      reference.getIndex()
                      << ", which is not found in the frame graph it lives in.";
            return;
        }
        resource->readPasses.emplace(passIndex);
        std::set<u32>& passes = resource->writePasses;
        if (!passes.empty()) {
            frameGraph->independent.erase(passIndex);
            for (u32 writer : passes) {
                mustRunAfter(
                    writer,
                    vk::AccessFlagBits::eMemoryRead,
                    vk::AccessFlagBits::eNoneKHR,
                    static_cast<vk::DependencyFlags>(0)
                );
            }
        }
    }

    RenderPassDescriptor::RenderPassDescriptor(
        u32 passIndex,
        FrameGraph* frameGraph
    ) : passIndex(passIndex), frameGraph(frameGraph) {}

    bool RenderPassDescriptor::tryFindResource(
        const FrameResourceReference& reference,
        un::FrameResource** resource
    ) {
        auto index = reference.getIndex();
        auto resources = frameGraph->resources;
        if (index >= resources.size()) {
            *resource = nullptr;
            return false;
        } else {
            *resource = &resources[index];
            return true;
        }
    }

    un::RenderPass* RenderPassDescriptor::getPass() {
        return frameGraph->vertex(passIndex);
    }

    void RenderPassDescriptor::mustRunAfter(
        u32 pass,
        vk::AccessFlags srcFlags,
        vk::AccessFlags dstFlags,
        vk::DependencyFlags flags
    ) {
        frameGraph->connect(
            pass, passIndex,
            un::RenderPassDependency()
        );
        frameGraph->connect(
            passIndex, pass,
            un::RenderPassDependency()
        );
    }

    void RenderPassDescriptor::mustRunAfter(
        const RenderPassDescriptor& pass,
        vk::AccessFlags srcFlags,
        vk::AccessFlags dstFlags,
        vk::DependencyFlags flags
    ) {
        mustRunAfter(pass.passIndex, srcFlags, dstFlags, flags);
    }

    void RenderPassDescriptor::usesAttachment(
        std::size_t attachmentIndex,
        vk::ImageLayout layout
    ) {
        getPass()->usedAttachments.emplace_back(attachmentIndex, layout);
    }

    void RenderPassDescriptor::usesColorAttachment(
        std::size_t attachmentIndex,
        vk::ImageLayout layout
    ) {
        getPass()->colorAttachments.emplace_back(attachmentIndex, layout);
    }

    FrameResource::FrameResource(std::string name) : name(std::move(name)) {}

    RenderPass::RenderPass(
        un::Renderer& renderer,
        std::string name,
        vk::PipelineStageFlags stageFlags
    ) : name(std::move(name)),
        stageFlags(stageFlags),
        commandBuffer(
            renderer,
            renderer.getGlobalPool(),
            vk::CommandBufferLevel::eSecondary
        ) {}

    const std::string& RenderPass::getName() const {
        return name;
    }


    void RenderPass::end() {
        /*commandBuffer->endRenderPass();*/
        commandBuffer->end();
    }

    vk::CommandBuffer RenderPass::begin(
        const vk::CommandBufferInheritanceInfo* info,
        const BakedFrameGraph& frameGraph,
        vk::Rect2D renderArea,
        vk::Framebuffer framebuffer
    ) const {
        commandBuffer->begin(
            vk::CommandBufferBeginInfo(
                (vk::CommandBufferUsageFlags) vk::CommandBufferUsageFlagBits::eOneTimeSubmit |
                vk::CommandBufferUsageFlagBits::eRenderPassContinue,
                info
            )
        );
        /* std::vector<vk::ClearValue> clears;
        for (const auto& item : frameGraph.getFrameGraph().getAttachments()) {
            clears.emplace_back(item.getClear());
        }
       commandBuffer->beginRenderPass(
            vk::RenderPassBeginInfo(
                frameGraph.getRenderPass(),
                framebuffer,
                renderArea,
                clears
            ),
            vk::SubpassContents::eSecondaryCommandBuffers
        );*/
        return *commandBuffer;
    }

    const std::vector<vk::AttachmentReference>& RenderPass::getUsedAttachments() const {
        return usedAttachments;
    }

    u32 FrameResourceReference::getIndex() const {
        return index;
    }

    FrameResource* FrameResourceReference::getResource() const {
        return resource;
    }

    un::RenderPassDescriptor FrameGraph::addPass(
        un::Renderer& renderer,
        const std::string& passName,
        vk::PipelineStageFlags flags
    ) {
        un::RenderPass pass(renderer, passName, flags);
        u32 index = push(std::move(pass));
        independent.emplace(index);
        return {index, this};
    }

    un::RenderPassDescriptor FrameGraph::addPass(
        un::Renderer& renderer,
        const std::string& passName,
        vk::PipelineStageFlags flags,
        un::RenderPass** result
    ) {
        auto descriptor = addPass(renderer, passName, flags);
        *result = descriptor.getPass();
        return descriptor;
    }

    un::FrameResourceReference FrameGraph::addResource(const std::string& resourceName) {
        size_t index = resources.size();
        auto& reference = resources.emplace_back(resourceName);
        return un::FrameResourceReference(index, &reference);
    }

    const std::vector<un::Attachment>& FrameGraph::getAttachments() const {
        return attachments;
    }

    FrameResourceReference::FrameResourceReference(
        u32 index,
        FrameResource* resource
    ) : index(index), resource(resource) {}

    BakedFrameGraph::BakedFrameGraph(
        un::FrameGraph&& graph,
        un::Renderer& renderer
    ) : frameGraph(std::move(graph)) {
        std::vector<vk::SubpassDescription> subPasses;
        std::vector<vk::SubpassDependency> subPassesDependencies;
        subPasses.reserve(frameGraph.size());
        for (auto[vertex, index] : frameGraph.all_vertices()) {
            subPasses.emplace_back(
                (vk::SubpassDescriptionFlags) 0,
                vk::PipelineBindPoint::eGraphics,
                vertex->getUsedAttachments(),
                vertex->getColorAttachments(),
                vertex->getResolveAttachments()
            );
            for (auto[neighborIndex, edge]:frameGraph.edges_from(index)) {
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
        for (const auto& item : frameGraph.attachments) {
            attachments.emplace_back(item.getDescription());
        }

        renderPass = renderer.getVirtualDevice().createRenderPass(
            vk::RenderPassCreateInfo(
                (vk::RenderPassCreateFlags) 0,
                attachments,
                subPasses,
                subPassesDependencies
            )
        );
    }


    const vk::RenderPass& BakedFrameGraph::getRenderPass() const {
        return renderPass;
    }

    const FrameGraph& BakedFrameGraph::getFrameGraph() const {
        return frameGraph;
    }

    un::FrameGraph& BakedFrameGraph::getFrameGraph() {
        return frameGraph;
    }

    const std::vector<vk::AttachmentReference>& RenderPass::getColorAttachments() const {
        return colorAttachments;
    }

    const std::vector<vk::AttachmentReference>&
    RenderPass::getResolveAttachments() const {
        return resolveAttachments;
    }

    const std::vector<vk::AttachmentReference>& RenderPass::getDepthAttachments() const {
        return depthAttachments;
    }

    const vk::PipelineStageFlags& RenderPass::getStageFlags() const {
        return stageFlags;
    }

    vk::CommandBuffer RenderPass::getCommandBuffer() const {
        return *commandBuffer;
    }

    RenderPassDependency::Type RenderPassDependency::getType() const {
        return type;
    }

    RenderPassDependency::RenderPassDependency(
        const vk::AccessFlags& srcFlags,
        const vk::AccessFlags& dstFlags,
        const vk::DependencyFlags& dependencyFlags
    ) : type(un::RenderPassDependency::Type::eUses),
        srcFlags(srcFlags),
        dstFlags(dstFlags),
        dependencyFlags(dependencyFlags) {}

    RenderPassDependency::RenderPassDependency() : type(un::RenderPassDependency::Type::eProvides) {}

    const vk::AccessFlags& RenderPassDependency::getSrcFlags() const {
        return srcFlags;
    }

    const vk::AccessFlags& RenderPassDependency::getDstFlags() const {
        return dstFlags;
    }

    const vk::DependencyFlags& RenderPassDependency::getDependencyFlags() const {
        return dependencyFlags;
    }

    Attachment::Attachment(
        const vk::AttachmentDescription& description,
        const vk::ClearValue& clear
    ) : description(description), clear(clear) {}

    const vk::AttachmentDescription& Attachment::getDescription() const {
        return description;
    }

    const vk::ClearValue& Attachment::getClear() const {
        return clear;
    }
}