#ifndef UNNECESSARYENGINE_SIMPLE_JOBS_H
#define UNNECESSARYENGINE_SIMPLE_JOBS_H

#include <unnecessary/jobs/job_system.h>
#include <unnecessary/jobs/workers/worker.h>

namespace un {
    class JobWorker;

    typedef un::Job<JobWorker> SimpleJob;

    class JobWorker : public un::AbstractJobWorker<SimpleJob> {
    public:
        JobWorker(std::size_t index, bool autostart);
    };

    class SimpleJobSystem : public un::JobSystem<JobWorker> {
    public:
        SimpleJobSystem(size_t numWorkers, bool autoStart);

        SimpleJobSystem(bool autoStart);

        void start();
    };
}
#endif
