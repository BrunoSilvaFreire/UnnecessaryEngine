#ifndef UNNECESSARYENGINE_EVENTS_H
#define UNNECESSARYENGINE_EVENTS_H

#include <memory>
#include <vector>
#include <chrono>
#include <string>
#include <unnecessary/def.h>
#include <unnecessary/jobs/job.h>

namespace un {
    enum profiler_event_type : u32 {
        worker_started,
        worker_exited,
        worker_sleeping,
        worker_awaken,
        job_started,
        job_finished,
        job_enqueued,
        job_unlocked
    };

    class event_meta {
    public:
        virtual ~event_meta();

        virtual profiler_event_type get_event_type() const = 0;

        virtual const std::string get_label() const = 0;

        virtual bool try_get_handle(job_handle& handle) const {
            return false;
        }
    };
};
#endif
