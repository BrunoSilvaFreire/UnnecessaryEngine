#include <unnecessary/systems/world.h>
#include <unnecessary/systems/system.h>
#include <unnecessary/systems/run_system_job.h>
#include <unnecessary/systems/parallel_system_data.h>
#include <unnecessary/jobs/job_chain.h>
#include <unnecessary/misc/benchmark.h>
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
        un::Chronometer<> chronometer;
        un::JobChain chain(jobSystem, false);
        for (ParallelSystemData *data : systems) {
            un::RunSystemJob &job = data->getJob();
            job.setDeltaTime(delta);
            u32 id;
            chain.enqueue(&id, &job);
            jobIds[data->getSystem()] = id;
        }
        for (ParallelSystemData *data : systems) {
            auto &dependencies = data->getDependencies();
            u32 id = jobIds[data->getSystem()];
            if (!dependencies.empty()) {

                for (un::System *dependency : dependencies) {
                    chain.after(jobIds[dependency], id);
                }
            } else {
                chain.immediately(id);
            }
        }
        std::condition_variable variable;
        std::mutex mutex;
        std::unique_lock lock(mutex);
        chain.onFinished([&variable](un::JobWorker *) {
            variable.notify_one();
        });
        chain.dispatch();
        variable.wait(lock);
        u32 cpuFrameTimeMillis = chronometer.stop();
        u32 totalMSForFrame = static_cast<u32>(1000.0F / targetFPS);
        if (totalMSForFrame > cpuFrameTimeMillis) {
            u64 msToSleep = totalMSForFrame - cpuFrameTimeMillis;
            std::this_thread::sleep_for(std::chrono::milliseconds(msToSleep));
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