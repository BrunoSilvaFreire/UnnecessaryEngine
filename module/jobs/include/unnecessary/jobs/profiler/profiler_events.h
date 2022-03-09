#ifndef UNNECESSARYENGINE_PROFILER_EVENTS_H
#define UNNECESSARYENGINE_PROFILER_EVENTS_H

#include <unnecessary/jobs/profiler/event_history.h>
#include <unnecessary/jobs/job.h>

namespace un {
    template<typename TJob>
    class JobMeta : public EventMeta {
    public:
        JobMeta(const TJob* const job, un::JobHandle jobHandle, std::string metaLabel)
            : jobLabel(job->getName()), jobHandle(jobHandle), EventMeta(metaLabel) {
        }

    private:
        std::string jobLabel;
        un::JobHandle jobHandle;
    };

    template<typename TJob>
    class JobStartedMeta : public JobMeta<TJob> {
    public:
        JobStartedMeta(const TJob* const job, JobHandle jobHandle) : JobMeta<TJob>(
            job,
            jobHandle,
            "job_started"
        ) {}
    };

    template<typename TJob>
    class JobFinishedMeta : public JobMeta<TJob> {
    public:
        JobFinishedMeta(const TJob* const job, JobHandle jobHandle) : JobMeta<TJob>(
            job,
            jobHandle,
            "job_finished"
        ) {}
    };

    class WorkerSleepingMeta : public EventMeta {
    public:
        WorkerSleepingMeta();
    };

    class WorkerStartMeta : public EventMeta {
    public:
        WorkerStartMeta();
    };

    class WorkerExitedMeta : public EventMeta {
    public:
        WorkerExitedMeta();
    };

    class WorkerAwakeMeta : public EventMeta {
    public:
        WorkerAwakeMeta();
    };
}
#endif
