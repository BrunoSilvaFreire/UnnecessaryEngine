#include <entt/entt.hpp>
#include <unnecessary/def.h>
#include <unnecessary/application.h>

#ifndef UNNECESSARYENGINE_WORLD_H
#define UNNECESSARYENGINE_WORLD_H
namespace un {
    class System;

    class ParallelSystemData;

    typedef AdjacencyList<ParallelSystemData, bool> SystemGraph;

    class World {
    private:
        u32 targetFPS = 165;
        un::SystemGraph systems;
        std::unordered_map<System *, u32> jobIds;
        entt::registry registry;
        un::JobSystem *jobSystem;
    public:
        entt::registry &getRegistry();

        template<typename... Component, typename... Exclude>
        entt::basic_view<
                entt::entity,
                entt::exclude_t<Exclude...>,
                Component...
        > view(entt::exclude_t<Exclude...> = {}) {
            return registry.view<Component..., Exclude...>();
        }

        template<typename T, typename ...Args>
        u32 addSystem(Args... args) {
            return addSystem(new T(args...));
        };

        u32 addSystem(System *system);

        void systemMustRunAfter(u32 system, u32 after);

        explicit World(un::Application &application);

        void step(f32 delta);

        virtual ~World();

        template<typename ...T>
        entt::entity createEntity() {
            entt::entity entity = registry.create();
            for_types<T...>([&](auto t) {
                using t_type = typename decltype(t)::type;
                registry.emplace<t_type>(entity);
            });
            return entity;
        }
    };

}
#endif
