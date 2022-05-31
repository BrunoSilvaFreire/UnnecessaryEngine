#ifndef UNNECESSARYENGINE_WORKER_RECORDER_H
#define UNNECESSARYENGINE_WORKER_RECORDER_H

#include <concepts>
#include <type_traits>
#include <unnecessary/jobs/workers/worker.h>
#include "unnecessary/jobs/recorder/data/recorder_events.h"

namespace un {
    template<typename TWorkerType>
    concept IsWorkerType = std::is_base_of_v<
        un::AbstractJobWorker<typename TWorkerType::JobType>,
        TWorkerType
    >;


    template<typename TWorker>
    class WorkerRecorder {
    public:
        typedef TWorker WorkerType;
    private:
        EventHistory _history;
        WorkerType* _worker;
    public:
        const EventHistory& getHistory() const {
            return _history;
        }

        WorkerType* getWorker() const {
            return _worker;
        }

        WorkerRecorder(WorkerType* type) {
            _worker = type;
            using JobType = typename WorkerType::JobType;
            type->onExited() += [this]() {
                _history.record(std::make_unique<un::WorkerExitedMeta>());
            };
            type->onSleeping() += [this]() {
                _history.record(std::make_unique<un::WorkerSleepingMeta>());
            };
            type->onAwaken() += [this]() {
                _history.record(std::make_unique<un::WorkerAwakeMeta>());
            };
            type->onStarted() += [this]() {
                _history.record(std::make_unique<un::WorkerStartMeta>());
            };
            type->onFetched() += [this](JobType* job, un::JobHandle handle) {
                _history.record(
                    std::make_unique<un::JobStartedMeta<JobType>>(
                        job,
                        handle
                    ));
            };
            type->onExecuted() += [this](JobType* job, un::JobHandle handle) {
                _history.record(
                    std::make_unique<un::JobFinishedMeta<JobType>>(
                        job,
                        handle
                    ));
            };
            type->onEnqueued() += [this](JobType* job, un::JobHandle handle) {
                _history.record(
                    std::make_unique<un::JobEnqueuedMeta<JobType>>(
                        job,
                        handle
                    )
                );
            };
        }
    };
}
#endif
