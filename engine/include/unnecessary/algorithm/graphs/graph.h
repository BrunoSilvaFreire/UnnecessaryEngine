#ifndef UNNECESSARYENGINE_GRAPH_H
#define UNNECESSARYENGINE_GRAPH_H

#include <vector>
#include <tuple>

namespace un {
    template<typename V, typename E>
    class AGraph {
    public:

        virtual u32 addVertex(V vertex) = 0;

        virtual void removeVertex(u32 index) = 0;

        virtual V &operator[](u32 index) = 0;

        virtual E &edge(u32 from, u32 to) = 0;

        virtual void clearVertices() = 0;

        virtual void clearEdges() = 0;

        virtual void disconnect(u32 from, u32 to) = 0;

        virtual void connect(u32 from, u32 to, E edge) = 0;

        virtual bool isOutOfBounds(u32 vertexIndex) = 0;

        virtual void clear() = 0;

        virtual u32 getSize() = 0;

        virtual void resize(u32 newSize) = 0;

        virtual std::vector<std::tuple<u32, E>> edgesFrom(u32 index) = 0;
    };
}

#endif
