#include <unnecessary/systems/world.h>
#include <unnecessary/systems/system.h>
#include <unnecessary/systems/run_system_job.h>
#include <unnecessary/systems/parallel_system_data.h>
#include <unordered_set>
#include <cmath>

namespace un {
    World::World(un::Application &app) : registry(), jobSystem(&app.getJobSystem()) {
        app.getOnPool() += [this](f32 delta) {
            step(delta);
        };
    }

    World::~World() {
//        app.getOnPool() -= &step;
    }

    void World::step(f32 delta) {
        auto start = std::chrono::high_resolution_clock::now();
        std::unordered_set<u32> ableToStart;
        for (ParallelSystemData *data : systems) {
            un::RunSystemJob &job = data->getJob();
            job.setDeltaTime(delta);
            u32 id = jobSystem->enqueue(&job, false);
            jobIds[data->getSystem()] = id;
            ableToStart.emplace(id);
        }
        for (ParallelSystemData *data : systems) {
            auto &dependencies = data->getDependencies();
            if (!dependencies.empty()) {
                u32 id = jobIds[data->getSystem()];
                ableToStart.erase(id);
                for (un::System *dependency : dependencies) {
                    jobSystem->addDependency(id, jobIds[dependency]);
                }
            }
        }
        for (u32 job : ableToStart) {
            jobSystem->markForExecution(job);
        }
        auto end = std::chrono::high_resolution_clock::now();
        f32 timeLeft = 1.0F / targetFPS;
        f32 frameTime = (end - start).count() / 1000000000.0F;
        timeLeft -= frameTime;
        if (timeLeft > 0) {
            u64 msToSleep = std::lround(timeLeft * 1000);
            if (msToSleep > 0) {
                std::this_thread::sleep_for(std::chrono::milliseconds(msToSleep));
            }
        }
    }

    entt::registry &World::getRegistry() {
        return registry;
    }

    u32 World::addSystem(System *system) {
        return systems.push(ParallelSystemData(system, this));
    }

    void World::systemMustRunAfter(u32 system, u32 after) {
        systems.connect(after, system, true);
    }
}