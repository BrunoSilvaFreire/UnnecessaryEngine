#ifndef UNNECESSARYENGINE_GRAPH_EXPLORATION_H
#define UNNECESSARYENGINE_GRAPH_EXPLORATION_H

#include <grapphs/graph.h>
#include <grapphs/adjacency_list.h>
#include <unnecessary/jobs/job.h>
#include <unnecessary/jobs/worker_chain.h>
#include <concepts>

namespace un {
    template<typename t_graph>
    class graph_explorer {
    public:
        using graph_type = t_graph;
        using vertex_type = typename graph_type::VertexType;
        using edge_type = typename graph_type::EdgeType;
        using index_type = typename graph_type::IndexType;
        using vertex_processor = std::function<
            void(
                index_type index,
                const vertex_type& vertex
            )
        >;
        using edge_processor = std::function<
            void(
                index_type from,
                index_type to,
                const edge_type& edge
            )
        >;

    private:
        vertex_processor _perVertex;
        edge_processor _perEdge;
        std::condition_variable _variable;
        std::mutex _remainingMutex;
        std::mutex _completedMutex;
        std::set<index_type> _remaining;
        std::set<index_type> _assigned;

    public:
        graph_explorer(
            const graph_type& graph,
            const vertex_processor& perVertex,
            const edge_processor& perEdge
        ) : _perVertex(perVertex),
            _perEdge(perEdge) {
            for (auto [vertex, index] : graph.all_vertices()) {
                _remaining.emplace(index);
            }
        }

        void vertex(index_type index, const vertex_type& vertexType) {
            _perVertex(index, vertexType);
            {
                std::lock_guard<std::mutex> lock(_remainingMutex);
                _remaining.erase(index);
                if (_remaining.empty()) {
                    _variable.notify_one();
                }
            }
        }

        void edge(index_type from, index_type to, edge_type& edgeType) {
            _perEdge(from, to, edgeType);
        }

        bool try_claim(index_type toExplore) {
            std::lock_guard<std::mutex> lock(_remainingMutex);
            bool canClaim =
                _remaining.contains(toExplore) && !_assigned.contains(toExplore);
            if (canClaim) {
                _assigned.emplace(toExplore);
            }
            return canClaim;
        }

        void complete() {
            if (_remaining.empty()) {
                return;
            }
            {
                std::unique_lock<std::mutex> lock(_completedMutex);
                _variable.wait(lock);
            }
        }
    };

    template<
        typename t_graph,
        typename t_job_system
    > requires gpp::is_graph<t_graph>
    class explore_graph_vertex_job : public simple_job {
    public:
        using vertex_type = typename t_graph::VertexType;
        using edge_type = typename t_graph::EdgeType;
        using graph_processor = graph_explorer<t_graph>;

    private:
        std::size_t _index;
        const t_graph* _graph;
        graph_processor* _explorer;
        t_job_system* _jobSystem;

    public:
        explore_graph_vertex_job(
            size_t index,
            const t_graph* graph,
            graph_processor* explorer,
            t_job_system* jobSystem,
            bool firstJob
        ) : _index(index), _graph(graph), _explorer(explorer), _jobSystem(jobSystem) {
            _name = std::string("Explore node ") + std::to_string(index);
            if (firstJob) {
                if (!explorer->try_claim(index)) {
                    throw std::runtime_error(
                        "Created first graph exploration job, but explorer says vertex is already claimed"
                    );
                }
            }
        }

        ~explore_graph_vertex_job() override {
        }

        void operator()(worker_type* worker) override {
            const vertex_type* vertex = _graph->vertex(_index);
            _explorer->vertex(_index, *vertex);
            worker_chain<job_worker> chain;
            for (auto [otherIndex, edge] : _graph->edges_from(_index)) {
                if (otherIndex == _index) {
                    continue;
                }
                if (!_explorer->try_claim(otherIndex)) {
                    continue;
                }
                //                LOG(INFO) << otherIndex << " claimed by " << worker->getIndex();
                _explorer->edge(_index, otherIndex, edge);
                chain.immediately<explore_graph_vertex_job<t_graph, t_job_system>>(
                    otherIndex,
                    _graph,
                    _explorer,
                    _jobSystem,
                    false
                );
            }
            chain.submit<t_job_system>(_jobSystem);
        }
    };
}
#endif
