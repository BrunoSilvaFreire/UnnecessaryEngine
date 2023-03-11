#include <unnecessary/jobs/simple_jobs.h>

namespace un {
    job_worker::job_worker(
        std::size_t index,
        std::size_t core,
        bool autostart
    ) : job_worker_mixin(
        index,
        core
    ) {
    }
}
