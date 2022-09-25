#include <unnecessary/jobs/simple_jobs.h>

namespace un {

    JobWorker::JobWorker(
        std::size_t index,
        std::size_t core,
        bool autostart
    ) : JobWorkerMixin(
        index,
        core
    ) { }
}