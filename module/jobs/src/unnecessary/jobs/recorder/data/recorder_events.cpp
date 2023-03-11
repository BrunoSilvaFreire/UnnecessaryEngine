#include "unnecessary/jobs/recorder/data/recorder_events.h"

namespace un {
    const std::string worker_sleeping_meta::get_label() const {
        return "worker_sleeping";
    }

    profiler_event_type worker_sleeping_meta::get_event_type() const {
        return worker_sleeping;
    }

    const std::string worker_start_meta::get_label() const {
        return "worker_started";
    }

    profiler_event_type worker_start_meta::get_event_type() const {
        return worker_started;
    }

    const std::string worker_exited_meta::get_label() const {
        return "worker_exited";
    }

    profiler_event_type worker_exited_meta::get_event_type() const {
        return worker_exited;
    }

    const std::string worker_awake_meta::get_label() const {
        return "worker_awake";
    }

    profiler_event_type worker_awake_meta::get_event_type() const {
        return worker_awaken;
    }
}
