#ifndef UNNECESSARYENGINE_JOBS_PROFILER_EVENTS_H
#define UNNECESSARYENGINE_JOBS_PROFILER_EVENTS_H

#include <memory>
#include <vector>
#include <chrono>
#include <string>
#include <unnecessary/def.h>
#include <unnecessary/jobs/job.h>

namespace un {
    enum ProfilerEventType : u32 {
        eWorkerStarted,
        eWorkerExited,
        eWorkerSleeping,
        eWorkerAwaken,
        eJobStarted,
        eJobFinished,
        eJobEnqueued,
        eJobUnlocked
    };

    class EventMeta {
    public:
        virtual ~EventMeta();

        virtual un::ProfilerEventType getEventType() const = 0;

        virtual const std::string getLabel() const = 0;

        virtual bool tryGetHandle(un::JobHandle& handle) const {
            return false;
        }
    };

};
#endif
