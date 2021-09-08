#include <unnecessary/systems/run_system_job.h>
#include <unnecessary/systems/system.h>

namespace un {
    RunSystemJob::RunSystemJob(
        un::World* world,
        un::System* system,
        f32 deltaTime
    ) : world(world), system(system), deltaTime(deltaTime) {}

    void RunSystemJob::operator()(un::JobWorker* worker) {
        system->step(*world, deltaTime, worker);
        LOG(INFO) << "Finished running system " << system;
    }

/*    un::System* RunSystemJob::getSystem() const {
        return system;
    }*/
}
