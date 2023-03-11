#ifndef UNNECESSARYENGINE_EVENT_HISTORY_H
#define UNNECESSARYENGINE_EVENT_HISTORY_H

#include <memory>
#include <vector>
#include <chrono>
#include <string>
#include <mutex>
#include <unnecessary/jobs/recorder/data/events.h>

namespace un {
    class profiler_event {
    public:
        profiler_event(
            std::chrono::high_resolution_clock::time_point time,
            std::unique_ptr<event_meta> meta
        );

        profiler_event(profiler_event&& moved);

        profiler_event(const profiler_event& copy) = delete;

        std::chrono::high_resolution_clock::time_point get_time() const;

        const std::unique_ptr<event_meta>& get_meta() const;

        bool operator<(const profiler_event& rhs) const;

        bool operator>(const profiler_event& rhs) const;

        bool operator<=(const profiler_event& rhs) const;

        bool operator>=(const profiler_event& rhs) const;

    protected:
        std::chrono::high_resolution_clock::time_point _time;
        std::unique_ptr<event_meta> _meta;
    };

    class event_history {
    private:
        std::mutex _queueMutex;
        std::vector<profiler_event> _events;

    public:
        event_history();

        const std::vector<profiler_event>& get_events() const;

        void record(std::unique_ptr<event_meta>&& meta);
    };
}
#endif
