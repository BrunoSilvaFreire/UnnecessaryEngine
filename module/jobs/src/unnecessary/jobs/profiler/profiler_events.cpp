#include <unnecessary/jobs/profiler/profiler_events.h>

namespace un {

    WorkerSleepingMeta::WorkerSleepingMeta() : EventMeta("sleeping") {}

    WorkerExitedMeta::WorkerExitedMeta() : EventMeta("exited") {}

    WorkerAwakeMeta::WorkerAwakeMeta() : EventMeta("awake") {}

    WorkerStartMeta::WorkerStartMeta() : EventMeta("start") {}
}