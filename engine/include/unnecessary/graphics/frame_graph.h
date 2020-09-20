#ifndef UNNECESSARYENGINE_FRAME_GRAPH_H
#define UNNECESSARYENGINE_FRAME_GRAPH_H

#include <vulkan/vulkan.hpp>
#include <boost/graph/adjacency_list.hpp>

namespace un {
    class FrameNode {
    private:
        std::string name;
        vk::CommandBuffer buffer;
    public:
        FrameNode(std::string name, vk::CommandBuffer buffer);
    };

    typedef boost::adjacency_list<
            boost::vecS,
            boost::vecS,
            boost::directedS,
            FrameNode,
            boost::no_property
    > FrameGraphBase;

    class FrameGraph : public FrameGraphBase {
    public:
        FrameGraph(
                const vk::Device &device,
                const vk::CommandPool &pool
        );
    };
}
#endif
