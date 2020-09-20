#include <unnecessary/world.h>
#include <unnecessary/systems/system.h>

namespace un {
    World::World(un::Application &app) : registry() {
        app.getOnPool() += [this](f32 delta) {
            step(delta);
        };
    }

    World::~World() {
//        app.getOnPool() -= &step;
    }

    void World::step(f32 delta) {
        for (System *system : systems) {
            system->step(*this, delta);
        }
    }

    entt::registry &World::getRegistry() {
        return registry;
    }

}