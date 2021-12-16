#include <unnecessary/logging.h>
#include "unnecessary/jobs/workers/worker.h"
#include <unnecessary/jobs/job.h>

namespace un {


    JobWorker::JobWorker(
        size_t index,
        bool autostart,
        const JobProvider<JobType>& provider,
        const JobNotifier<JobType>& notifier
    ) : AbstractJobWorker(index, autostart, provider, notifier) {}
}