#include <entt/entt.hpp>
#include <unnecessary/def.h>
#include "application.h"

#ifndef UNNECESSARYENGINE_WORLD_H
#define UNNECESSARYENGINE_WORLD_H
namespace un {
    class System;

    class World {
    private:
        std::vector<System *> systems;
        entt::registry registry;
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
        void addSystem(Args... args) {
            systems.push_back(new T(args...));
        };

        explicit World(un::Application &application);

        void step(f32 delta);

        virtual ~World();
    };
}
#endif
