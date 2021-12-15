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

    template<typename T>
    std::string vertex_properties(
        const T* vertexType
    ) {
        return "";
    }

    template<typename VertexType, typename IndexType = u32>
    class DependencyGraph
        : private gpp::AdjacencyList<VertexType, DependencyType, IndexType> {
    public:
        typedef std::function<void(IndexType index, const VertexType& vertex)> Explorer;

    private:
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
        ) {
            std::vector<IndexType> dependencies;
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
            dot << "digraph {" << std::endl;
            for (auto[vertexPtr, index] : DependencyGraph<VertexType>::all_vertices()) {
                dot << index << "[label =\"#" << index << ": "
                    << un::to_string(*vertexPtr)
                    << "\" fontname=\"monospace\"" <<
                    vertex_properties(vertexPtr) <<
                    "];" << std::endl;
            }
            for (auto[vertex, index] : DependencyGraph<VertexType>::all_vertices()) {
                for (auto[otherIndex, edge] : DependencyGraph<VertexType>::edges_from(
                    index
                )) {
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
                IndexType ptr;
                DependencyView* owner;
            public:
                DependencyIterator(
                    IndexType index,
                    DependencyView* owner
                ) : ptr(index), owner(owner) {
                }

                std::pair<IndexType, VertexType*> operator*();


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
            DependencyGraph<VertexType>* owner;


        public:
            DependencyView(
                DependencyGraph<VertexType>* owner,
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
            gpp::AdjacencyList<VertexType, DependencyType, IndexType>::remove(index);
        }

        bool disconnect(
            IndexType from,
            IndexType to
        ) override {
            return gpp::AdjacencyList<VertexType, DependencyType, IndexType>::disconnect(
                from,
                to
            );
        }

        template<typename ...Args>
        IndexType add(Args... args) {
            VertexType vertex(args...);
            IndexType id = gpp::AdjacencyList<VertexType, DependencyType, IndexType>::push(
                std::move(
                    vertex
                )
            );
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

        DependencyView dependenciesOf(IndexType index) {
            return DependencyView(
                this,
                findDependencies(index, un::DependencyType::eUses)
            );
        }

        DependencyView dependantsOn(IndexType index) {
            return DependencyView(
                this,
                findDependencies(index, un::DependencyType::eUsed)
            );
        }

        void each(
            const Explorer& explorer
        ) const {
            std::queue<IndexType> open;
            std::set<IndexType> currentlyEnqueued;
            std::set<IndexType> visited;
            for (IndexType item : independent) {
                open.push(item);
            }
            while (!open.empty()) {
                IndexType next = open.front();
                open.pop();
                visit(next, open, visited, currentlyEnqueued, explorer);
            }
        }

#ifdef __GCC__
        __attribute__((noinline))
#endif

        VertexType* operator[](IndexType index) {
            return this->vertex(index);
        }

        const VertexType* operator[](IndexType index) const {
            return this->vertex(index);
        }

        bool tryGetVertex(IndexType index, VertexType& output) {
            return this->try_get_vertex(index, output);
        }
    };
    template<typename VertexType, typename IndexType>
    std::pair<IndexType, VertexType*>
    un::DependencyGraph<VertexType, IndexType>::DependencyView::DependencyIterator::operator*() {
        IndexType dependency = owner->dependencies[ptr];
        return std::pair(dependency, owner->owner->vertex(dependency));
    }

}
#endif
