#include <unnecessary/jobs/recorder/data/event_history.h>

namespace un {
    profiler_event::profiler_event(
        std::chrono::high_resolution_clock::time_point time,
        std::unique_ptr<event_meta> meta
    ) : _time(time), _meta(std::move(meta)) {
    }

    profiler_event::profiler_event(
        profiler_event&& moved
    ) : _time(std::move(moved._time)),
        _meta(std::move(moved._meta)) {
    }

    std::chrono::high_resolution_clock::time_point profiler_event::get_time() const {
        return _time;
    }

    bool profiler_event::operator<(const profiler_event& rhs) const {
        return _time < rhs._time;
    }

    bool profiler_event::operator>(const profiler_event& rhs) const {
        return rhs < *this;
    }

    bool profiler_event::operator<=(const profiler_event& rhs) const {
        return !(rhs < *this);
    }

    bool profiler_event::operator>=(const profiler_event& rhs) const {
        return !(*this < rhs);
    }

    const std::unique_ptr<event_meta>& profiler_event::get_meta() const {
        return _meta;
    }

    void event_history::record(std::unique_ptr<event_meta>&& meta) {
        std::chrono::high_resolution_clock::time_point point = std::chrono::high_resolution_clock::now();
        {
            std::lock_guard<std::mutex> lock(_queueMutex);
            _events.emplace_back(point, std::move(meta));
        }
    }

    event_history::event_history() : _events() {
    }

    const std::vector<profiler_event>& event_history::get_events() const {
        return _events;
    }
}
