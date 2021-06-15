#include <unnecessary/systems/run_system_job.h>

namespace un {
    RunSystemJob::RunSystemJob(
            un::World *world,
            un::System *system
    ) : world(world), system(system), deltaTime(0) {}

    void RunSystemJob::operator()(un::JobWorker *worker) {
        system->step(*world, deltaTime);
    }

    void RunSystemJob::setDeltaTime(f32 deltaTime) {
        RunSystemJob::deltaTime = deltaTime;
    }
}
