#ifndef UNNECESSARYENGINE_DEPENDENCY_GRAPH_H
#define UNNECESSARYENGINE_DEPENDENCY_GRAPH_H

#include <unnecessary/def.h>
#include <unnecessary/strings.h>
#include <filesystem>
#include <grapphs/adjacency_list.h>
#include <grapphs/flood.h>
#include <iostream>
#include <fstream>
#include <string>

namespace un {
    enum DependencyType {
        eUses,
        eProvides
    };

#define GRAPPH_TYPES(VType, EType)  using VertexType = #VType;\
                                    using EdgeType = #EType;\
                                    using u32 = u32;

    template<typename VertexType>
    class DependencyGraph : private gpp::AdjacencyList<VertexType, DependencyType, u32> {
    public:
        typedef std::function<void(u32 index, const VertexType& vertex)> Explorer;

    private:
        std::set<u32> independent;

        void visit(
            u32 index,
            std::queue<u32>& open,
            std::set<u32>& visited,
            std::set<u32>& currentlyEnqueued,
            const Explorer& explorer
        ) const {
            for (auto[neighbor, edge] : DependencyGraph<VertexType>::edges_from(index)) {
                if (visited.contains(neighbor) || currentlyEnqueued.contains(neighbor)) {
                    continue;
                }
                if (edge == un::DependencyType::eUses) {
                    //Did not visit dependency yet
                    visit(neighbor, open, visited, currentlyEnqueued, explorer);
                } else {
                    currentlyEnqueued.emplace(neighbor);
                    open.push(neighbor);
                }
            }
            explorer(index, *DependencyGraph<VertexType>::vertex(index));
            visited.emplace(index);
        }

    protected:
        virtual std::string getProperties(const VertexType* vertexType) const {
            return "";
        }

    public:

        std::string toDot() const {
            std::stringstream dot;
            dot << "digraph {";
            for (auto[vertexPtr, index] : DependencyGraph<VertexType>::all_vertices()) {
                dot << index << "[label =\"#" << index << ": "
                    << un::to_string(*vertexPtr)
                    << "\" fontname=\"monospace\"" <<
                    getProperties(vertexPtr) <<
                    "];" << std::endl;
            }
            for (auto[vertex, index] : DependencyGraph<VertexType>::all_vertices()) {
                for (auto[
                    otherIndex,
                    edge
                    ] : DependencyGraph<VertexType>::edges_from(index)) {
                    if (edge == un::DependencyType::eUses) {
                        dot << index << "->" << otherIndex << ";" << std::endl;
                    }
                }
            }
            dot << "}";
            return dot.str();
        }

        void saveToDot(const std::filesystem::path& toSaveInto) const {
            std::string content = toDot();
            std::ofstream stream(toSaveInto.string());
            stream << content;
            stream.close();
        }

        class DependencyView {
        public:
            class DependencyIterator {
            private:
                u32 ptr;
                std::vector<u32> dependencies;
                DependencyGraph<VertexType>* owner;
            public:
                DependencyIterator(
                    u32 index,
                    DependencyGraph<VertexType>* owner
                ) : ptr(0), dependencies(), owner(owner) {
                    for (auto[dependency, edge] : owner->node(index).connections()) {
                        dependencies.emplace_back(dependency);
                    }
                }

                u32 numEntries() {
                    return dependencies.size();
                }

                std::pair<u32, VertexType*> operator*() {
                    u32 dependency = dependencies[ptr];
                    return std::pair(dependency, owner->vertex(dependency));
                }

                void operator++() {
                    ptr++;
                }

                bool operator==(u32 other) {
                    return ptr == other;
                }

                bool operator!=(u32 other) {
                    return ptr != other;
                }
            };

        private:
            DependencyIterator iterator;
            u32 last;
        public:
            DependencyView(
                u32 index,
                DependencyGraph<VertexType>* owner
            ) : iterator(index, owner) {
                last = iterator.numEntries();
            }

            DependencyIterator begin() {
                return iterator;
            }

            u32 end() {
                return last;
            }


        };

        void addDependency(u32 from, u32 to) {
            independent.erase(from);
            DependencyGraph<VertexType>::connect(from, to, un::DependencyType::eUses);
            DependencyGraph<VertexType>::connect(to, from, un::DependencyType::eProvides);
        }

        void remove(u32 index) {
            gpp::AdjacencyList<VertexType, DependencyType, u32>::remove(index);
        }

        template<typename ...Args>
        u32 add(Args... args) {
            VertexType vertex(args...);
            u32 id = gpp::AdjacencyList<VertexType, DependencyType, u32>::push(
                std::move(
                    vertex
                )
            );
            independent.emplace(id);
            return id;
        };

        DependencyView dependenciesOf(u32 index) {
            return DependencyView(index, this);
        }

        void each(
            const Explorer& explorer
        ) const {
            std::queue<u32> open;
            std::set<u32> currentlyEnqueued;
            std::set<u32> visited;
            for (u32 item : independent) {
                open.push(item);
            }
            while (!open.empty()) {
                u32 next = open.front();
                open.pop();
                visit(next, open, visited, currentlyEnqueued, explorer);
            }
        }


    };

}
#endif
