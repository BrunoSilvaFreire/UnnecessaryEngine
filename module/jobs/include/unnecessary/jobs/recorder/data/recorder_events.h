#ifndef UNNECESSARYENGINE_RECORDER_EVENTS_H
#define UNNECESSARYENGINE_RECORDER_EVENTS_H

#include "unnecessary/jobs/job.h"
#include "events.h"
#include "event_history.h"

namespace un {
    template<typename TJob>
    class job_meta : public event_meta {
    public:
        job_meta(const TJob* const job, job_handle jobHandle)
            : _jobLabel(job->get_name()), _jobHandle(jobHandle) {
        }

        const std::string get_label() const override {
            return _jobLabel;
        }

        bool try_get_handle(job_handle& handle) const override {
            handle = _jobHandle;
            return true;
        }

    private:
        std::string _jobLabel;
        job_handle _jobHandle;
    };

    template<typename TJob>
    class job_started_meta : public job_meta<TJob> {
    public:
        job_started_meta(const TJob* const job, job_handle jobHandle) : job_meta<TJob>(
            job,
            jobHandle
        ) {
        }

        profiler_event_type get_event_type() const override {
            return job_started;
        }
    };

    template<typename TJob>
    class job_finished_meta : public job_meta<TJob> {
    public:
        job_finished_meta(const TJob* const job, job_handle jobHandle) : job_meta<TJob>(
            job,
            jobHandle
        ) {
        }

        profiler_event_type get_event_type() const override {
            return job_finished;
        }
    };

    template<typename TJob>
    class job_enqueued_meta : public job_meta<TJob> {
    public:
        job_enqueued_meta(
            const TJob* const job,
            job_handle jobHandle
        ) : job_meta<TJob>(job, jobHandle) {
        }

        profiler_event_type get_event_type() const override {
            return job_enqueued;
        }
    };

    class worker_sleeping_meta : public event_meta {
    public:
        profiler_event_type get_event_type() const override;

        const std::string get_label() const override;
    };

    class worker_start_meta : public event_meta {
    public:
        const std::string get_label() const override;

        profiler_event_type get_event_type() const override;
    };

    class worker_exited_meta : public event_meta {
    public:
        const std::string get_label() const override;

        profiler_event_type get_event_type() const override;
    };

    class worker_awake_meta : public event_meta {
    public:
        const std::string get_label() const override;

        profiler_event_type get_event_type() const override;
    };
}
#endif
