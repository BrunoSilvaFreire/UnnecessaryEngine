#ifndef UNNECESSARYENGINE_DEPENDENCY_GRAPH_H
#define UNNECESSARYENGINE_DEPENDENCY_GRAPH_H

#include <unnecessary/def.h>
#include <unnecessary/strings.h>
#include <filesystem>
#include <grapphs/adjacency_list.h>
#include <grapphs/algorithms/flood.h>
#include <grapphs/algorithms/rlo_traversal.h>
#include <iostream>
#include <fstream>
#include <string>

namespace un {
    enum DependencyType {
        eUses,
        eUsed
    };

    template<>
    std::string to_string(const un::DependencyType& type);

    std::ostream& operator<<(std::ostream& stream, const un::DependencyType& dependencyType);


    template<typename VertexType, typename IndexType = u32>
    class DependencyGraph {
    public:
        typedef std::function<void(IndexType index, const VertexType& vertex)> Explorer;
        typedef gpp::AdjacencyList<VertexType, DependencyType, IndexType> InnerGraph;
    protected:
        InnerGraph graph;
        std::set<IndexType> independent;

        void visit(
            IndexType index,
            std::queue<IndexType>& open,
            std::set<IndexType>& visited,
            std::set<IndexType>& currentlyEnqueued,
            const Explorer& explorer
        ) const {
            for (auto [neighbor, edge] : DependencyGraph<VertexType>::edges_from(index)) {
                if (visited.contains(neighbor)) {
                    continue;
                }
                if (edge == un::DependencyType::eUses) {
                    //Did not visit dependency yet
                    visit(neighbor, open, visited, currentlyEnqueued, explorer);
                } else {
                    if (!currentlyEnqueued.contains(neighbor)) {
                        currentlyEnqueued.emplace(neighbor);
                        open.push(neighbor);
                    }
                }
            }
            // Check if we were recursively checked by someone else
            if (!visited.contains(index)) {
                explorer(index, *DependencyGraph<VertexType>::vertex(index));
                visited.emplace(index);
            }
        }


        std::vector<IndexType> findDependencies(
            IndexType index,
            un::DependencyType expected
        ) const {
            std::vector<IndexType> dependencies;
            for (auto [dependency, edge] : graph.node(index).connections()) {
                if (edge != expected) {
                    continue;
                }
                dependencies.emplace_back(dependency);
            }
            return dependencies;
        }

    public:
        class DependencyView {

        private:
            std::vector<IndexType> dependencies;
            const DependencyGraph<VertexType>* owner;


        public:
            typedef typename std::vector<IndexType>::iterator IteratorType;

            DependencyView(
                const DependencyGraph<VertexType>* owner,
                std::vector<IndexType> dependencies
            ) : owner(owner),
                dependencies(dependencies) {
            }

            IteratorType begin() {
                return dependencies.begin();
            }

            IteratorType end() {
                return dependencies.end();
            }

            const std::vector<IndexType>& getDependencies() const {
                return dependencies;
            }
        };

        un::DependencyType dependency(IndexType from, IndexType to) {
            return *this->edge(from, to);
        }

        void addDependency(IndexType from, IndexType to) {
            independent.erase(from);
            graph.connect(from, to, un::DependencyType::eUses);
            graph.connect(to, from, un::DependencyType::eUsed);
        }

        void remove(IndexType index) {
            graph.remove(index);
        }

        bool disconnect(
            IndexType from,
            IndexType to
        ) {
            return graph.disconnect(from, to);
        }

        template<typename ...Args>
        IndexType add(Args... args) {
            VertexType vertex(args...);
            IndexType id = graph.push(std::move(vertex));
            independent.emplace(id);
            return id;
        };

        IndexType add(VertexType&& vertex) {
            IndexType id = graph.push(std::move(vertex));
            independent.emplace(id);
            return id;
        };

        DependencyView allConnectionsOf(IndexType index) {
            std::vector<IndexType> all;
            for (auto [dependency, edge] : this->node(index).connections()) {
                all.emplace_back(dependency);
            }
            return DependencyView(
                this,
                all
            );
        }

        DependencyView dependenciesOf(IndexType index) const {
            return DependencyView(
                this,
                findDependencies(index, un::DependencyType::eUses)
            );
        }

        DependencyView dependantsOn(IndexType index) const {
            return DependencyView(
                this,
                findDependencies(index, un::DependencyType::eUsed)
            );
        }

        std::size_t getSize() const {
            return graph.size();
        };

        UN_AGGRESSIVE_INLINE void each(
            const std::function<void(
                IndexType index,
                const VertexType& vertex
            )>& perVertex,
            const std::function<void(
                IndexType from,
                IndexType to,
                const un::DependencyType& dependency
            )>& perEdge
        ) const {
            std::queue<IndexType> open;
            std::set<IndexType> currentlyEnqueued;
            std::set<IndexType> visited;
            for (IndexType item : independent) {
                open.emplace(item);
            }
            while (!open.empty()) {
                IndexType next = open.front();
                const VertexType* vertex = graph.vertex(next);
                perVertex(next, *vertex);
                visited.emplace(next);
                for (IndexType other : dependantsOn(next)) {
                    const un::DependencyType& dependency = *graph.edge(next, other);
                    perEdge(next, other, dependency);
                    if (!visited.contains(other)) {
                        open.emplace(other);
                    }
                }
                open.pop();
            }
        }

        /**
         * rlo = Reverse Level Order traversal
         * @see https://www.geeksforgeeks.org/reverse-level-order-traversal
         * @param perVertex
         * @param perEdge
         */
        UN_AGGRESSIVE_INLINE void each_rlo(
            const std::function<void(
                IndexType index
            )>& perVertex,
            const std::function<void(
                IndexType from,
                IndexType to
            )>& perEdge
        ) const {
            gpp::reverse_level_order_traverse(graph, independent, perVertex, perEdge);
        }

        std::vector<std::pair<IndexType, const VertexType*>> get_rlo_sequence() const {
            std::vector<std::pair<IndexType, const VertexType*>> sequence;
            each_rlo(
                [&](u32 index) {
                    sequence.emplace_back(index, graph.vertex(index));
                },
                [](u32 from, u32 to) { }
            );
            return sequence;
        }

        const InnerGraph& getInnerGraph() const {
            return graph;
        }

        InnerGraph& getInnerGraph() {
            return graph;
        }

        VertexType* operator[](IndexType index) {
            return graph.vertex(index);
        }

        const VertexType* operator[](IndexType index) const {
            return graph.vertex(index);
        }

        bool tryGetVertex(IndexType index, VertexType& output) {
            return graph.try_get_vertex(index, output);
        }
    };

}
#endif
