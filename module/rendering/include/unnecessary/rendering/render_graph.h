#ifndef UNNECESSARYENGINE_RENDER_GRAPH_H
#define UNNECESSARYENGINE_RENDER_GRAPH_H

#include <unnecessary/graphs/dependency_graph.h>
#include <unnecessary/rendering/render_pass_description.h>

namespace un {
    class RenderGraph : public un::DependencyGraph<un::RenderPass> {

    };
};
#endif //UNNECESSARYENGINE_RENDER_GRAPH_H
