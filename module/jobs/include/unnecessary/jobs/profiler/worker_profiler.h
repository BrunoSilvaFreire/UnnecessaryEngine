#ifndef UNNECESSARYENGINE_WORKER_PROFILER_H
#define UNNECESSARYENGINE_WORKER_PROFILER_H

#include <concepts>
#include <type_traits>
#include <unnecessary/jobs/workers/worker.h>
#include <unnecessary/jobs/profiler/profiler_events.h>

namespace un {
    template<typename TWorkerType>
    concept IsWorkerType = std::is_base_of_v<
        un::AbstractJobWorker<typename TWorkerType::JobType>,
        TWorkerType
    >;


    template<typename TWorker>
    class WorkerProfiler  {
    public:
        typedef TWorker WorkerType;
    private:
        EventHistory _history;
    public:
        WorkerProfiler(WorkerType* type) {
            using JobType = typename WorkerType::JobType;
            type->onExited() += [this]() {
                _history.record(new un::WorkerExitedMeta());
            };
            type->onSleeping() += [this]() {
                _history.record(new un::WorkerSleepingMeta());
            };
            type->onAwaken() += [this]() {
                _history.record(new un::WorkerAwakeMeta());
            };
            type->onStarted() += [this]() {
                _history.record(new un::WorkerStartMeta());
            };
            type->onFetched() += [this](JobType* job, un::JobHandle handle) {
                _history.record(new un::JobStartedMeta<JobType>(job, handle));
            };
            type->onExecuted() += [this](JobType* job, un::JobHandle handle) {
                _history.record(new un::JobFinishedMeta<JobType>(job, handle));
            };
        }
    };
}
#endif
