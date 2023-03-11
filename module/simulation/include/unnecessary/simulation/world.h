#ifndef UNNECESSARYENGINE_WORLD_H
#define UNNECESSARYENGINE_WORLD_H

#include <entt/entt.hpp>
#include <unnecessary/def.h>
#include <unnecessary/application/application.h>
#include <unnecessary/jobs/worker_chain.h>
#include <unnecessary/simulation/simulation_graph.h>

namespace un {
    class system;

    class layer {
    };

    class world : public entt::registry {
    private:
        constexpr static size_t k_num_layers = 32;
        std::array<layer, k_num_layers> _layers;
        simulation_graph _simulationGraph;

    public:
        void add_system(system* system) {
        }

        template<typename... t_T>
        std::tuple<entt::entity, t_T* ...> create_with() {
            entt::entity entity = create();
            return std::make_tuple<entt::entity, t_T* ...>(entity, &emplace<t_T...>(entity));
        }

        void step(f32 deltaTime, simulation_chain& chain);
    };

    template<typename t_job_system>
    class simulator {
    private:
        t_job_system* _jobSystem;
        world* _world;

    public:
        simulator(
            t_job_system* jobSystem,
            world* world
        ) : _jobSystem(jobSystem),
            _world(world) {
        }

        void apply(application& application) {
            application.get_fixed_loop().early(
                [this, &application]() {
                    simulation_chain chain;
                    _world->step(1 / application.get_fixed_frame_rate(), chain);
                    chain.submit(_jobSystem);
                }
            );
        }
    };
}
#endif
