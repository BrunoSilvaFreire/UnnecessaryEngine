#ifndef UNNECESSARYENGINE_SIMPLE_JOBS_H
#define UNNECESSARYENGINE_SIMPLE_JOBS_H

#include <unnecessary/jobs/job_system.h>
#include <unnecessary/jobs/workers/worker.h>

namespace un {
    class JobWorker;

    typedef un::Job<JobWorker> SimpleJob;

    class JobWorker : public un::JobWorkerMixin<SimpleJob> {
    public:
        JobWorker(std::size_t index, std::size_t core, bool autostart);
    };

    typedef un::JobSystem<un::JobWorker> SimpleJobSystem;

}
#endif
