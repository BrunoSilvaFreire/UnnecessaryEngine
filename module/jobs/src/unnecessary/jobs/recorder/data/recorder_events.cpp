#include "unnecessary/jobs/recorder/data/recorder_events.h"

namespace un {

    const std::string WorkerSleepingMeta::getLabel() const {
        return "worker_sleeping";
    }

    ProfilerEventType WorkerSleepingMeta::getEventType() const {
        return un::ProfilerEventType::eWorkerSleeping;
    }

    const std::string WorkerStartMeta::getLabel() const {
        return "worker_started";
    }

    ProfilerEventType WorkerStartMeta::getEventType() const {
        return un::ProfilerEventType::eWorkerStarted;
    }

    const std::string WorkerExitedMeta::getLabel() const {
        return "worker_exited";
    }

    ProfilerEventType WorkerExitedMeta::getEventType() const {
        return un::ProfilerEventType::eWorkerExited;
    }

    const std::string WorkerAwakeMeta::getLabel() const {
        return "worker_awake";
    }

    ProfilerEventType WorkerAwakeMeta::getEventType() const {
        return un::ProfilerEventType::eWorkerAwaken;
    }
}