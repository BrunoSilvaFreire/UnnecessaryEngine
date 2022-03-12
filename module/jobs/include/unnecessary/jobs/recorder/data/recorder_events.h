#ifndef UNNECESSARYENGINE_RECORDER_EVENTS_H
#define UNNECESSARYENGINE_RECORDER_EVENTS_H

#include "unnecessary/jobs/job.h"
#include "events.h"
#include "event_history.h"

namespace un {
    template<typename TJob>
    class JobMeta : public EventMeta {
    public:
        JobMeta(const TJob* const job, un::JobHandle jobHandle)
            : jobLabel(job->getName()), jobHandle(jobHandle) {
        }

        const std::string getLabel() const {
            return jobLabel;
        }

        bool tryGetHandle(JobHandle& handle) const override {
            handle = jobHandle;
            return true;
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
            jobHandle
        ) {}


        ProfilerEventType getEventType() const override {
            return un::ProfilerEventType::eJobStarted;
        }
    };

    template<typename TJob>
    class JobFinishedMeta : public JobMeta<TJob> {
    public:
        JobFinishedMeta(const TJob* const job, JobHandle jobHandle) : JobMeta<TJob>(
            job,
            jobHandle
        ) {}

        ProfilerEventType getEventType() const override {
            return un::ProfilerEventType::eJobFinished;
        }

    };

    class WorkerSleepingMeta : public EventMeta {
    public:
        ProfilerEventType getEventType() const override;

        const std::string getLabel() const override;
    };

    class WorkerStartMeta : public EventMeta {
    public:
        const std::string getLabel() const override;

        ProfilerEventType getEventType() const override;
    };

    class WorkerExitedMeta : public EventMeta {
    public:
        const std::string getLabel() const override;

        ProfilerEventType getEventType() const override;
    };

    class WorkerAwakeMeta : public EventMeta {
    public:
        const std::string getLabel() const override;

        ProfilerEventType getEventType() const override;
    };
}
#endif
