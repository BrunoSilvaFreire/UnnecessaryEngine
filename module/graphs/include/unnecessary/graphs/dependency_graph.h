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
    enum dependency_type {
        uses,
        used
    };

    template<>
    std::string to_string(const dependency_type& type);

    std::ostream& operator<<(std::ostream& stream, const dependency_type& dependencyType);

    template<typename t_vertex, typename t_index = u32>
    class dependency_graph {
    public:
        using vertex_type = t_vertex;
        using index_type = t_index;
        using explorer = std::function<void(index_type index, const vertex_type& vertex)>;
        using inner_graph = gpp::adjacency_list<vertex_type, dependency_type, index_type>;

    protected:
        inner_graph _graph;
        std::set<index_type> _independent;

        void visit(
            index_type index,
            std::queue<index_type>& open,
            std::set<index_type>& visited,
            std::set<index_type>& currentlyEnqueued,
            const explorer& explorer
        ) const {
            for (auto [neighbor, edge] : dependency_graph<vertex_type>::edges_from(index)) {
                if (visited.contains(neighbor)) {
                    continue;
                }
                if (edge == uses) {
                    //Did not visit dependency yet
                    visit(neighbor, open, visited, currentlyEnqueued, explorer);
                }
                else {
                    if (!currentlyEnqueued.contains(neighbor)) {
                        currentlyEnqueued.emplace(neighbor);
                        open.push(neighbor);
                    }
                }
            }
            // Check if we were recursively checked by someone else
            if (!visited.contains(index)) {
                explorer(index, *dependency_graph<vertex_type>::vertex(index));
                visited.emplace(index);
            }
        }

        std::vector<index_type> find_dependencies(
            index_type index,
            dependency_type expected
        ) const {
            std::vector<index_type> dependencies;
            for (auto [dependency, edge] : _graph.node(index).connections()) {
                if (edge != expected) {
                    continue;
                }
                dependencies.emplace_back(dependency);
            }
            return dependencies;
        }

    public:
        class dependency_view {
        private:
            std::vector<index_type> _dependencies;
            const dependency_graph<vertex_type>* _owner;

        public:
            using iterator_type = typename std::vector<index_type>::iterator;

            dependency_view(
                const dependency_graph<vertex_type>* owner,
                std::vector<index_type> dependencies
            ) : _dependencies(dependencies),
                _owner(owner) {
            }

            iterator_type begin() {
                return _dependencies.begin();
            }

            iterator_type end() {
                return _dependencies.end();
            }

            const std::vector<index_type>& get_dependencies() const {
                return _dependencies;
            }
        };

        dependency_type dependency(index_type from, index_type to) {
            return *this->edge(from, to);
        }

        void add_dependency(index_type from, index_type to) {
            _independent.erase(from);
            _graph.connect(from, to, uses);
            _graph.connect(to, from, used);
        }

        void remove(index_type index) {
            _graph.remove(index);
        }

        bool disconnect(
            index_type from,
            index_type to
        ) {
            bool firstSuccess = _graph.disconnect(from, to);
            bool secondSuccess = _graph.disconnect(to, from);
            return firstSuccess && secondSuccess;
        }

        template<typename... Args>
        index_type add(Args... args) {
            vertex_type vertex(args...);
            index_type id = _graph.push(std::move(vertex));
            _independent.emplace(id);
            return id;
        };

        index_type add(vertex_type&& vertex) {
            index_type id = _graph.push(std::move(vertex));
            _independent.emplace(id);
            return id;
        };

        dependency_view all_connections_of(index_type index) {
            std::vector<index_type> all;
            for (auto [dependency, edge] : this->node(index).connections()) {
                all.emplace_back(dependency);
            }
            return dependency_view(
                this,
                all
            );
        }

        dependency_view dependencies_of(index_type index) const {
            return dependency_view(
                this,
                find_dependencies(index, uses)
            );
        }

        dependency_view dependants_on(index_type index) const {
            return dependency_view(
                this,
                find_dependencies(index, used)
            );
        }

        std::size_t get_size() const {
            return _graph.size();
        };

        UN_AGGRESSIVE_INLINE void each(
            const std::function<
                void(
                    index_type index,
                    const vertex_type& vertex
                )
            >& perVertex,
            const std::function<
                void(
                    index_type from,
                    index_type to,
                    const dependency_type& dependency
                )
            >& perEdge
        ) const {
            std::queue<index_type> open;
            std::set<index_type> currentlyEnqueued;
            std::set<index_type> visited;
            for (index_type item : _independent) {
                open.emplace(item);
            }
            while (!open.empty()) {
                index_type next = open.front();
                const vertex_type* vertex = _graph.vertex(next);
                perVertex(next, *vertex);
                visited.emplace(next);
                for (index_type other : dependants_on(next)) {
                    const dependency_type& dependency = *_graph.edge(next, other);
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
            const std::function<
                void(
                    index_type index
                )
            >& perVertex,
            const std::function<
                void(
                    index_type from,
                    index_type to
                )
            >& perEdge
        ) const {
            gpp::reverse_level_order_traverse(_graph, _independent, perVertex, perEdge);
        }

        std::vector<std::pair<index_type, const vertex_type*>> get_rlo_sequence() const {
            std::vector<std::pair<index_type, const vertex_type*>> sequence;
            each_rlo(
                [&](u32 index) {
                    sequence.emplace_back(index, _graph.vertex(index));
                },
                [](u32 from, u32 to) {
                }
            );
            return sequence;
        }

        const inner_graph& get_inner_graph() const {
            return _graph;
        }

        inner_graph& get_inner_graph() {
            return _graph;
        }

        vertex_type* operator[](index_type index) {
            return _graph.vertex(index);
        }

        const vertex_type* operator[](index_type index) const {
            return _graph.vertex(index);
        }

        bool try_get_vertex(index_type index, vertex_type& output) {
            return _graph.try_get_vertex(index, output);
        }
    };
}
#endif
