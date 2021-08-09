#ifndef UNNECESSARYENGINE_RENDER_GRAPH_H
#define UNNECESSARYENGINE_RENDER_GRAPH_H

#include <grapphs/adjacency_list.h>

namespace un {
    class RenderPass {

    };

    class RenderDependency {

    };

    class RenderGraph : public gpp::AdjacencyList<RenderPass, RenderDependency> {

    };
}
#endif
