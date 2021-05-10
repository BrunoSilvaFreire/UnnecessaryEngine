#include <unnecessary/systems/run_system_job.h>

namespace un {
    RunSystemJob::RunSystemJob(
            un::World *world,
            un::System *system
    ) : world(world), system(system), deltaTime(0) {}

    un::RunSystemJob::operator void() {
        system->step(*world, deltaTime);
    }

    void RunSystemJob::setDeltaTime(f32 deltaTime) {
        RunSystemJob::deltaTime = deltaTime;
    }
}
