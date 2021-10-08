#include <entt/entt.hpp>
#include <unnecessary/def.h>
#include <unnecessary/application.h>
#include <unnecessary/systems/simulation.h>

#include <grapphs/adjacency_list.h>

#ifndef UNNECESSARYENGINE_WORLD_H
#define UNNECESSARYENGINE_WORLD_H

namespace un {
    class SimpleSystem;

    class Simulation;

    class ParallelSystemData;

    typedef gpp::AdjacencyList<ParallelSystemData, bool> SystemGraph;

    class LightRegistry {
    private:
        std::vector<entt::entity> pointLights;
    public:
    };

    class World : public entt::registry {
    private:
        u32 targetFPS = 165;
        un::Simulation simulation;
        un::JobSystem* jobSystem;
    public:
        entt::registry& getRegistry();


        template<typename T, typename ...Args>
        T* addSystem(Args... args) {
            auto[_, system] = simulation.addSystem<T>(args...);
            return system;
        };

        explicit World(un::Application& application);

        void step(f32 delta);

        virtual ~World();

        template<typename ...T>
        entt::entity createEntity() {
            entt::entity entity = create();
            for_types<T...>(
                [&](auto t) {
                    using t_type = typename decltype(t)::type;
                    emplace<t_type>(entity);
                }
            );
            return entity;
        }

        const Simulation& getSimulation() const;
    };

}
#endif
