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
        eUsed
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
        virtual std::string getProperties(const VertexType* vertexType) const {
            return "";
        }

        std::vector<u32> findDependencies(
            u32 index,
            un::DependencyType expected
        ) {
            std::vector<u32> dependencies;
            for (auto[dependency, edge] : this->node(index).connections()) {
                if (edge != expected) {
                    continue;
                }
                dependencies.emplace_back(dependency);
            }
            return dependencies;
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
                DependencyView* owner;
            public:
                DependencyIterator(
                    u32 index,
                    DependencyView* owner
                ) : ptr(index), owner(owner) {
                }

                std::pair<u32, VertexType*> operator*();

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
            std::vector<u32> dependencies;
            DependencyGraph<VertexType>* owner;


        public:
            DependencyView(
                DependencyGraph<VertexType>* owner,
                std::vector<u32> dependencies
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
        };

        un::DependencyType dependency(u32 from, u32 to) {
            return *this->edge(from, to);
        }

        void addDependency(u32 from, u32 to) {
            independent.erase(from);
            DependencyGraph<VertexType>::connect(from, to, un::DependencyType::eUses);
            DependencyGraph<VertexType>::connect(to, from, un::DependencyType::eUsed);
        }

        void remove(u32 index) {
            gpp::AdjacencyList<VertexType, DependencyType, u32>::remove(index);
        }

        bool disconnect(
            u32 from,
            u32 to
        ) override {
            return gpp::AdjacencyList<VertexType, DependencyType, u32>::disconnect(
                from,
                to
            );
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

        DependencyView allConnectionsOf(u32 index) {
            std::vector<u32> all;
            for (auto[dependency, edge] : this->node(index).connections()) {
                all.emplace_back(dependency);
            }
            return DependencyView(
                this,
                all
            );
        }

        DependencyView dependenciesOf(u32 index) {
            return DependencyView(
                this,
                findDependencies(index, un::DependencyType::eUses)
            );
        }

        DependencyView dependantsOn(u32 index) {
            return DependencyView(
                this,
                findDependencies(index, un::DependencyType::eUsed)
            );
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

#ifdef __GCC__
        __attribute__((noinline))
#endif

        VertexType* operator[](u32 index) {
            return this->vertex(index);
        }

        const VertexType* operator[](u32 index) const {
            return this->vertex(index);
        }

        bool tryGetVertex(u32 index, VertexType& output) {
            return this->try_get_vertex(index, output);
        }
    };

    template<typename VertexType>
    std::pair<u32, VertexType*>
    DependencyGraph<VertexType>::DependencyView::DependencyIterator::operator*() {
        u32 dependency = owner->dependencies[ptr];
        return std::pair(dependency, owner->owner->vertex(dependency));
    }


}
#endif
