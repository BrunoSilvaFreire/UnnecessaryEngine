#include <entt/entt.hpp>
#include <unnecessary/def.h>
#include <unnecessary/application.h>
#include <unnecessary/systems/parallel_system_data.h>
#include <grapphs/adjacency_list.h>

#ifndef UNNECESSARYENGINE_WORLD_H
#define UNNECESSARYENGINE_WORLD_H

namespace un {
    class System;

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
        un::SystemGraph systems;
        std::unordered_map<System*, u32> jobIds;
        un::JobSystem* jobSystem;
    public:
        entt::registry& getRegistry();

        template<typename T, typename ...Args>
        u32 addSystem(Args... args) {
            return addSystem(new T(args...));
        };

        u32 addSystem(System* system);

        void systemMustRunAfter(u32 system, u32 after);

        explicit World(un::Application& application);

        void step(f32 delta);

        virtual ~World();

        const un::SystemGraph& getSystems() const;

        un::SystemGraph& getSystems();

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
    };

}
#endif
