#ifndef UNNECESSARYENGINE_WORLD_H
#define UNNECESSARYENGINE_WORLD_H

#include <entt/entt.hpp>
#include <unnecessary/def.h>
#include <unnecessary/application/application.h>
#include <unnecessary/jobs/worker_chain.h>
#include <unnecessary/simulation/simulation_graph.h>

namespace un {
    class System;

    class Layer {

    };

    class World : public entt::registry {
    private:
        constexpr static size_t kNumLayers = 32;
        std::array<un::Layer, kNumLayers> layers;
        un::SimulationGraph _simulationGraph;
    public:
        void addSystem(un::System* system) {

        }

        template<typename ...T>
        std::tuple<entt::entity, T* ...> createWith() {
            entt::entity entity = create();
            return std::make_tuple<entt::entity, T* ...>(entity, &emplace<T...>(entity));
        }

        void step(f32 deltaTime, un::SimulationChain& chain);

    };

    template<typename JobSystemType>
    class Simulator {
    private:
        JobSystemType* _jobSystem;
        un::World* world;
    public:
        Simulator(JobSystemType* jobSystem, World* world)
            : _jobSystem(jobSystem), world(world) { }

        void apply(un::Application& application) {
            application.getFixedLoop().early(
                [this, &application]() {
                    un::SimulationChain chain;
                    world->step(1 / application.getFixedFrameRate(), chain);
                    chain.submit(_jobSystem);
                }
            );
        }
    };
}
#endif