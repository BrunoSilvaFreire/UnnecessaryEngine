#include <unnecessary/systems/world.h>
#include <unnecessary/systems/system.h>
#include <unnecessary/misc/benchmark.h>

namespace un {
    World::World(un::Application& app) : jobSystem(&app.getJobSystem()) {
        app.getOnPool() += [this](f32 delta) {
            step(delta);
        };
        u32 upload = simulation.addStage(un::kUploadFrameData);
        u32 prepare = simulation.addStage(un::kPrepareFrame);
        u32 record = simulation.addStage(un::kRecordFrame);
        u32 dispatch = simulation.addStage(un::kDispatchFrame);
        u32 earlyGameplay = simulation.addStage(un::kEarlyGameplay);
        u32 lateGameplay = simulation.addStage(un::kLateGameplay);
        simulation.addDependency(dispatch, record);
        simulation.addDependency(upload, prepare);
        simulation.addDependency(record, upload);
        simulation.addDependency(lateGameplay, earlyGameplay);
        simulation.addDependency(prepare, lateGameplay);
    }

    World::~World() {
//        app.getOnPool() -= &step;
    }

    void World::step(f32 delta) {
        un::Chronometer<std::chrono::milliseconds> chronometer;
        un::JobChain chain(jobSystem, false);
        simulation.step(*this, delta, chain);
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

    const Simulation& World::getSimulation() const {
        return simulation;
    }
}