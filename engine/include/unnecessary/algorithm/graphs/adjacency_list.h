#ifndef UNNECESSARYENGINE_ADJACENCY_LIST_H
#define UNNECESSARYENGINE_ADJACENCY_LIST_H

#include <unnecessary/algorithm/graphs/graph.h>
#include <vector>
#include <queue>
#include <unordered_map>

namespace un {

    template<typename V, typename E>
    class AdjacencyList : public AGraph<V, E> {
    public:
        class Node {
        private:
            V vertex;
            std::unordered_map<u32, E> edges;
        public:
            Node() : vertex() {}

            explicit Node(V vertex) : vertex(vertex) {}

            void setVertex(V vertex) {
                Node::vertex = vertex;
                edges.clear();
            }

            V &getVertex() {
                return vertex;
            }

            std::unordered_map<u32, E> &getEdges() {
                return edges;
            }
        };

    private:
        std::queue<u32> unusedIndices;
        std::vector<Node> nodes;
    public:
        AdjacencyList() = default;

        std::vector<std::tuple<u32, E>> edgesFrom(u32 index) override {
            typedef typename std::unordered_map<u32, E>::value_type pair_type;
            Node &node = nodes[index];
            std::vector<std::tuple<u32, E>> results;
            for (pair_type &pair : node.getEdges()) {
                results.push_back(std::make_tuple(std::get<0>(pair), std::get<1>(pair)));
            }
            return results;
        }

        u32 addVertex(V vertex) override {
            if (unusedIndices.empty()) {
                u32 index = nodes.size();
                nodes.emplace_back(vertex);
                return index;
            } else {
                u32 index = unusedIndices.front();
                unusedIndices.pop();
                nodes[index].setVertex(vertex);
                return index;
            }
        }

        void removeVertex(u32 index) override {
            unusedIndices.push(index);
        }

        V &operator[](u32 index) override {
            return nodes[index].getVertex();
        }

        E &edge(u32 from, u32 to) override {
            Node &node = nodes[from];
            auto &edges = node.getEdges();
            return edges[to];
        }

        void clearVertices() override {
            clear();
        }

        void clearEdges() override {
            for (Node &node : nodes) {
                node.getEdges().clear();
            }
        }

        void disconnect(u32 from, u32 to) override {
            Node &node = nodes[from];
            node.getEdges().erase(to);
        }

        void connect(u32 from, u32 to, E edge) override {
            Node &node = nodes[from];
            node.getEdges()[to] = edge;
        }

        bool isOutOfBounds(u32 vertexIndex) override {
            return vertexIndex >= nodes.size();
        }

        void clear() override {
            nodes.clear();
        }

        u32 getSize() override {
            return nodes.size();
        }

        void resize(u32 newSize) override {
            nodes.resize(newSize);
        }
    };
}
#endif
