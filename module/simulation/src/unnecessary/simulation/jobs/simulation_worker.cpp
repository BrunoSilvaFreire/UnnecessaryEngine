#include <unnecessary/simulation/jobs/simulation_worker.h>

namespace un {
    simulation_worker::simulation_worker(
        size_t index,
        bool autostart
    ) : job_worker_mixin(index, autostart) {
    }
}
