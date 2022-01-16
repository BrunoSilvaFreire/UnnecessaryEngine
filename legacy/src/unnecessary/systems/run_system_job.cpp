#include <unnecessary/systems/run_system_job.h>
#include <unnecessary/systems/system.h>
#include "../../../include/unnecessary/systems/run_system_job.h"

namespace un {
    RunSystemJob::RunSystemJob(
        un::World* world,
        un::SimpleSystem* system,
        f32 deltaTime
    ) : world(world), system(system), deltaTime(deltaTime) {}

    void RunSystemJob::operator()(un::JobWorker* worker) {
        system->step(*world, <#initializer#>);
    }

/*    un::SimpleSystem* RunSystemJob::getSystem() const {
        return system;
    }*/
}
