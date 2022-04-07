#include <unnecessary/logging.h>
#include "unnecessary/jobs/workers/worker.h"
#include <unnecessary/jobs/job.h>

namespace un {


    JobWorker::JobWorker(
        size_t index,
        bool autostart
    ) : AbstractJobWorker(index, autostart) { }
}