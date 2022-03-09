#include <unnecessary/jobs/profiler/event_history.h>

namespace un {

    ProfilerEvent::~ProfilerEvent() {
        if (_meta != nullptr) {
            delete _meta;
        }
    }

    ProfilerEvent::ProfilerEvent(
        std::chrono::high_resolution_clock::time_point time,
        EventMeta* meta
    ) : time(time), _meta(meta) {

    }

    std::chrono::high_resolution_clock::time_point ProfilerEvent::getTime() const {
        return time;
    }

    bool ProfilerEvent::operator<(const ProfilerEvent& rhs) const {
        return time < rhs.time;
    }

    bool ProfilerEvent::operator>(const ProfilerEvent& rhs) const {
        return rhs < *this;
    }

    bool ProfilerEvent::operator<=(const ProfilerEvent& rhs) const {
        return !(rhs < *this);
    }

    bool ProfilerEvent::operator>=(const ProfilerEvent& rhs) const {
        return !(*this < rhs);
    }

    ProfilerEvent::ProfilerEvent(
        ProfilerEvent&& moved
    ) : time(std::move(moved.time)),
        _meta(std::move(moved._meta)) {

    }

    EventMeta::EventMeta(std::string label) : label(std::move(label)) {}

    const std::string& EventMeta::getLabel() const {
        return label;
    }

    EventMeta::~EventMeta() {

    }

    void EventHistory::record(EventMeta* meta) {
        std::chrono::high_resolution_clock::time_point point = std::chrono::high_resolution_clock::now();
        _events.emplace_back(point, meta);
    }

    EventHistory::EventHistory() : _events() {

    }

}