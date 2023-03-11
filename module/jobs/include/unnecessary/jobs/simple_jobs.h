#ifndef UNNECESSARYENGINE_SIMPLE_JOBS_H
#define UNNECESSARYENGINE_SIMPLE_JOBS_H

#include <unnecessary/jobs/job_system.h>
#include <unnecessary/jobs/workers/worker.h>

namespace un {
    class job_worker;

    using simple_job = job<job_worker>;

    class job_worker : public job_worker_mixin<simple_job> {
    public:
        job_worker(std::size_t index, std::size_t core, bool autostart);
    };

    using simple_job_system = job_system<job_worker>;
}
#endif
