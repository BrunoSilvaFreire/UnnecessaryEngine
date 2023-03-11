#ifndef UNNECESSARYENGINE_WORKER_RECORDER_H
#define UNNECESSARYENGINE_WORKER_RECORDER_H

#include <concepts>
#include <type_traits>
#include <unnecessary/jobs/workers/worker.h>
#include "unnecessary/jobs/recorder/data/recorder_events.h"

namespace un {
    template<typename t_worker>
    concept IsWorkerType = std::is_base_of_v<
        job_worker_mixin<typename t_worker::job_type>,
        t_worker
    >;

    template<typename t_worker>
    class worker_recorder {
    public:
        using worker_type = t_worker;

    private:
        event_history _history;
        worker_type* _worker;

    public:
        const event_history& get_history() const {
            return _history;
        }

        worker_type* get_worker() const {
            return _worker;
        }

        worker_recorder(worker_type* type) {
            _worker = type;
            using job_type = typename worker_type::job_type;
            type->on_exited() += [this]() {
                _history.record(std::make_unique<worker_exited_meta>());
            };
            type->on_sleeping() += [this]() {
                _history.record(std::make_unique<worker_sleeping_meta>());
            };
            type->on_awaken() += [this]() {
                _history.record(std::make_unique<worker_awake_meta>());
            };
            type->on_started() += [this]() {
                _history.record(std::make_unique<worker_start_meta>());
            };
            type->on_fetched() += [this](job_type* job, job_handle handle) {
                _history.record(
                    std::make_unique<job_started_meta<job_type>>(
                        job,
                        handle
                    ));
            };
            type->on_executed() += [this](job_type* job, job_handle handle) {
                _history.record(
                    std::make_unique<job_finished_meta<job_type>>(
                        job,
                        handle
                    ));
            };
            type->on_enqueued() += [this](job_type* job, job_handle handle) {
                _history.record(
                    std::make_unique<job_enqueued_meta<job_type>>(
                        job,
                        handle
                    )
                );
            };
        }
    };
}
#endif
