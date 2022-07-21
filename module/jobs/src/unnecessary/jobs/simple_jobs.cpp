#include <unnecessary/jobs/simple_jobs.h>

namespace un {

    JobWorker::JobWorker(
        std::size_t index,
        bool autostart
    ) : AbstractJobWorker(index, autostart) { }
}