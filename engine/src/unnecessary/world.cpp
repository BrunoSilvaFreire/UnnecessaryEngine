#include <algorithm>
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
        frameCounter++;
        std::condition_variable variable;
        std::mutex mutex;
        std::unique_lock lock(mutex);
        chain.onFinished(
            [&variable](un::JobWorker*) {
                variable.notify_one();
            }
        );

        jobSystem->tasks.saveToDot("jobs.dot");
        chain.dispatch();
        variable.wait(lock);
        u32 cpuFrameTimeMillis = chronometer.stop();
        u64 numTicks = latestUpdatesDurations.size();
        u64 update = frameCounter % numTicks;
        latestUpdatesDurations[update] = static_cast<float>(cpuFrameTimeMillis) / 1000;
        if (update == numTicks - 1) {
            float avgCpuTime = 0;
            for (u32 j = 0; j < numTicks; ++j) {
                avgCpuTime += latestUpdatesDurations[j];
            }
            avgCpuTime /= numTicks;
            LOG(INFO) << "Avg CPU Time (" << numTicks << " ticks):" << avgCpuTime;
        }
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