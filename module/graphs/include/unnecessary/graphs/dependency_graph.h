#ifndef UNNECESSARYENGINE_DEPENDENCY_GRAPH_H
#define UNNECESSARYENGINE_DEPENDENCY_GRAPH_H

#include <unnecessary/def.h>
#include <unnecessary/strings.h>
#include <filesystem>
#include <grapphs/adjacency_list.h>
#include <grapphs/algorithms/flood.h>
#include <iostream>
#include <fstream>
#include <string>

namespace un {
    enum DependencyType {
        eUses,
        eUsed
    };


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
            for (auto[neighbor, edge] : DependencyGraph<VertexType>::edges_from(index)) {
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

    protected:

        std::vector<IndexType> findDependencies(
            IndexType index,
            un::DependencyType expected
        ) const {
            std::vector<IndexType> dependencies;
            for (auto[dependency, edge] : graph.node(index).connections()) {
                if (edge != expected) {
                    continue;
                }
                dependencies.emplace_back(dependency);
            }
            return dependencies;
        }

    public:
        class DependencyView {
        public:
            class DependencyIterator {
            private:
                IndexType ptr;
                DependencyView* owner;
            public:
                DependencyIterator(
                    IndexType index,
                    DependencyView* owner
                ) : ptr(index), owner(owner) {
                }

                std::pair<IndexType, DependencyType> operator*();


                void operator++() {
                    ptr++;
                }

                bool operator==(DependencyIterator other) {
                    return ptr == other.ptr;
                }

                bool operator!=(DependencyIterator other) {
                    return ptr != other.ptr;
                }
            };

        private:
            DependencyIterator first, last;
            std::vector<IndexType> dependencies;
            const DependencyGraph<VertexType>* owner;


        public:
            DependencyView(
                const DependencyGraph<VertexType>* owner,
                std::vector<IndexType> dependencies
            ) : owner(owner),
                dependencies(dependencies),
                first(0, this),
                last(dependencies.size(), this) {
            }

            DependencyIterator begin() {
                return first;
            }

            DependencyIterator end() {
                return last;
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
            DependencyGraph<VertexType>::connect(from, to, un::DependencyType::eUses);
            DependencyGraph<VertexType>::connect(to, from, un::DependencyType::eUsed);
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

        DependencyView allConnectionsOf(IndexType index) {
            std::vector<IndexType> all;
            for (auto[dependency, edge] : this->node(index).connections()) {
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

        std::size_t getSize() {
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
                for (std::pair<IndexType, un::DependencyType> pair : dependantsOn(next)) {
                    IndexType other = pair.first;
                    un::DependencyType dependency = pair.second;
                    perEdge(next, other, dependency);
                    if (!visited.contains(other)) {
                        open.emplace(other);
                    }
                }
                open.pop();
            }
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

    template<typename VertexType, typename IndexType>
    std::pair<IndexType, DependencyType>
    un::DependencyGraph<VertexType, IndexType>::DependencyView::DependencyIterator::operator*() {
        IndexType dependency = owner->dependencies[ptr];
        return std::pair<IndexType, DependencyType>(
            dependency,
            *(owner->owner->graph.edge(ptr, dependency))
        );
    }

}
#endif
