#include <unnecessary/graphics/frame_graph.h>

#include <utility>

namespace un {
    FrameGraph::FrameGraph(
            const vk::Device &device,
            const vk::CommandPool &pool
            ) {
        /*
        vk::CommandBuffer presentBuffer;
        presentBuffer = device.allocateCommandBuffers(
                vk::CommandBufferAllocateInfo(
                        pool,
                        vk::CommandBufferLevel::ePrimary,
                        1
                )
        );
        FrameNode present("present", presentBuffer);
        boost::add_vertex( present, *this);
        */
    }

    FrameNode::FrameNode(
            std::string name,
            vk::CommandBuffer buffer
    ) : name(std::move(name)), buffer(buffer) {}
}