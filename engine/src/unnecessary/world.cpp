#include <unnecessary/systems/world.h>
#include <unnecessary/systems/system.h>
#include <unnecessary/misc/benchmark.h>

namespace un {
    World::World(un::Application& app) : jobSystem(&app.getJobSystem()) {
        app.getOnPool() += [this](f32 delta) {
            step(delta);
        };
    }

    World::~World() {
//        app.getOnPool() -= &step;
    }

    void World::step(f32 delta) {
        un::Chronometer<std::chrono::milliseconds> chronometer;
        un::JobChain chain(jobSystem, false);
        for (ParallelSystemData* data : systems) {
            un::RunSystemJob& job = data->getJob();
            job.setDeltaTime(delta);
            u32 id;
            chain.enqueue(&id, &job);
            jobIds[data->getSystem()] = id;
        }
        for (ParallelSystemData* data : systems) {
            auto& dependencies = data->getDependencies();
            u32 id = jobIds[data->getSystem()];
            if (!dependencies.empty()) {

                for (un::System* dependency : dependencies) {
                    chain.after(jobIds[dependency], id);
                }
            } else {
                chain.immediately(id);
            }
        }
        std::condition_variable variable;
        std::mutex mutex;
        std::unique_lock lock(mutex);
        chain.onFinished(
            [&variable](un::JobWorker*) {
                variable.notify_one();
            }
        );
        chain.dispatch();
        variable.wait(lock);
        u32 cpuFrameTimeMillis = chronometer.stop();
        u32 totalMSForFrame = static_cast<u32>(1000.0F / targetFPS);
        if (totalMSForFrame > cpuFrameTimeMillis) {
            u64 msToSleep = totalMSForFrame - cpuFrameTimeMillis;
            std::this_thread::sleep_for(std::chrono::milliseconds(msToSleep));
        }
    }

    entt::registry& World::getRegistry() {
        return *this;
    }

    u32 World::addSystem(System* system) {
        un::SystemDescriptor descriptor(this);
        system->describe(descriptor);
        const auto& problems = descriptor.getProblems();
        if (!problems.empty()) {
            LOG(FUCK)
                << "Unable to add system to world because of the following problems:";
            for (const un::Problem& problem : problems) {
                LOG(FUCK) << problem.getDescription();
            }
            return -1;
        }
        return systems.push(un::ParallelSystemData(this, system));
    }

    void World::systemMustRunAfter(u32 system, u32 after) {
        systems.connect(after, system, true);
    }

    const SystemGraph& World::getSystems() const {
        return systems;
    }

    un::SystemGraph& World::getSystems() {
        return systems;
    }
}