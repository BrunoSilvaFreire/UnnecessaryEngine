#include <unnecessary/jobs/job_system_builder.h>

namespace un {
    template<>
    JobSystemBuilder<un::SimpleJobSystem>::JobSystemBuilder(
    ) : allocationConfig(), autoStart(true) {
        fillWorkers<un::JobWorker>();
    }
}