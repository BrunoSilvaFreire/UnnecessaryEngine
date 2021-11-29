#ifndef UNNECESSARYENGINE_BATCH_H
#define UNNECESSARYENGINE_BATCH_H

#include <entt/entt.hpp>
#include <unordered_set>
#include <unordered_map>
#include <unnecessary/graphics/material.h>
#include <queue>
#include <algorithm>
#include <functional>

namespace un {
    class Batch {
    private:
        std::unordered_set<entt::entity> entities;
    public:
        Batch() = default;

        void include(entt::entity entity) {
            entities.emplace(entity);
        }

        void exclude(entt::entity exclude) {

        }
    };

    class Batcher {
    private:
        std::unordered_set<un::Identifier, un::Batch> byPipeline;
        std::unordered_set<un::Identifier, un::Batch> byMesh;
        std::vector<un::Pipeline*> pipelines;
    public:
        Batcher() : pipelines() {

        }

        void includePipeline(un::Pipeline* pipeline) {
            pipelines.push_back(pipeline);
            std::push_heap(
                pipelines.begin(),
                pipelines.end(),
                [](un::Pipeline* first, un::Pipeline* other) {
                    return first->getOrder() > other->getOrder();
                }
            );
        }

        void forEachPipeline(std::function<void(un::Pipeline*)> block) {
        }
    };
}
#endif
