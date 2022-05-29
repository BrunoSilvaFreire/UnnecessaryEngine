#ifndef UNNECESSARYENGINE_GRAPH_EXPLORATION_H
#define UNNECESSARYENGINE_GRAPH_EXPLORATION_H

#include <grapphs/graph.h>
#include <grapphs/adjacency_list.h>
#include <unnecessary/jobs/job.h>
#include <unnecessary/jobs/worker_chain.h>
#include <concepts>

namespace un {
    template<typename TGraph>
    class GraphExplorer {
    public:
        using GraphType = TGraph;
        using VertexType = typename GraphType::VertexType;
        using EdgeType = typename GraphType::EdgeType;
        using IndexType = typename GraphType::IndexType;
        typedef std::function<void(
            IndexType index,
            const VertexType& vertex
        )> VertexProcessor;
        typedef std::function<void(
            IndexType from,
            IndexType to,
            const EdgeType& edge
        )> EdgeProcessor;
    private:
        VertexProcessor perVertex;
        EdgeProcessor perEdge;
        std::condition_variable variable;
        std::mutex remainingMutex;
        std::mutex completedMutex;
        std::set<IndexType> remaining;
        std::set<IndexType> assigned;
    public:
        GraphExplorer(
            const GraphType& graph,
            const VertexProcessor& perVertex,
            const EdgeProcessor& perEdge
        ) : perVertex(perVertex),
            perEdge(perEdge) {
            for (auto [vertex, index] : graph.all_vertices()) {
                remaining.emplace(index);
            }
        }

        void vertex(IndexType index, const VertexType& vertexType) {
            perVertex(index, vertexType);
            {
                std::lock_guard<std::mutex> lock(remainingMutex);
                remaining.erase(index);
                if (remaining.empty()) {
                    variable.notify_one();
                }
            }
        }

        void edge(IndexType from, IndexType to, EdgeType& edgeType) {
            perEdge(from, to, edgeType);
        }

        bool tryClaim(IndexType toExplore) {
            std::lock_guard<std::mutex> lock(remainingMutex);
            bool canClaim =
                remaining.contains(toExplore) && !assigned.contains(toExplore);
            if (canClaim) {
                assigned.emplace(toExplore);
            }
            return canClaim;
        }

        void complete() {
            if (remaining.empty()) {
                return;
            }
            {
                std::unique_lock<std::mutex> lock(completedMutex);
                variable.wait(lock);
            }
        }
    };


    template<
        typename TGraph,
        typename TJobSystem
    > requires gpp::is_graph<TGraph>
    class ExploreGraphVertexJob : public SimpleJob {
    public:
        using VertexType = typename TGraph::VertexType;
        using EdgeType = typename TGraph::EdgeType;
        typedef GraphExplorer<TGraph> GraphProcessor;
    private:
        std::size_t index;
        const TGraph* graph;
        GraphProcessor* explorer;
        TJobSystem* jobSystem;
    public:
        ExploreGraphVertexJob(
            size_t index,
            const TGraph* graph,
            GraphProcessor* explorer,
            TJobSystem* jobSystem,
            bool firstJob
        ) : index(index), graph(graph), explorer(explorer), jobSystem(jobSystem) {
            name = std::string("Explore node ") + std::to_string(index);
            if (firstJob) {
                if (!explorer->tryClaim(index)) {
                    throw std::runtime_error(
                        "Created first graph exploration job, but explorer says vertex is already claimed"
                    );
                }
            }
        }

        ~ExploreGraphVertexJob() {

        }

        void operator()(WorkerType* worker) override {
            const VertexType* vertex = graph->vertex(index);
            explorer->vertex(index, *vertex);
            un::WorkerChain<JobWorker> chain;
            for (auto [otherIndex, edge] : graph->edges_from(index)) {
                if (otherIndex == index) {
                    continue;
                }
                if (!explorer->tryClaim(otherIndex)) {
                    continue;
                }
//                LOG(INFO) << otherIndex << " claimed by " << worker->getIndex();
                explorer->edge(index, otherIndex, edge);
                chain.template immediately<un::ExploreGraphVertexJob<TGraph, TJobSystem>>(
                    otherIndex,
                    graph,
                    explorer,
                    jobSystem,
                    false
                );
            }
            chain.template submit<TJobSystem>(jobSystem);
        }
    };
}
#endif
