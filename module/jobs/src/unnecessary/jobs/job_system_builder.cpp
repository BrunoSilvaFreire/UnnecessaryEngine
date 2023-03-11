#include <unnecessary/jobs/job_system_builder.h>

namespace un {
    template<>
    job_system_builder<simple_job_system>::job_system_builder(
    ) : _allocationConfig(), _autoStart(true) {
        fill_workers<job_worker>();
    }
}
