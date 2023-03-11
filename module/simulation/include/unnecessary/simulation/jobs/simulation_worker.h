#ifndef UNNECESSARYENGINE_SIMULATION_WORKER_H
#define UNNECESSARYENGINE_SIMULATION_WORKER_H

#include <unnecessary/jobs/workers/worker.h>

namespace un {
    class simulation_worker;

    using simulation_job = job<simulation_worker>;

    class simulation_worker : public job_worker_mixin<simulation_job> {
    private:
    public:
        simulation_worker(
            size_t index,
            bool autostart
        );
    };
}
#endif
