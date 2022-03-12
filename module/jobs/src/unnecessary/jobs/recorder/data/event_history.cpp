#include <unnecessary/jobs/recorder/data/event_history.h>

namespace un {

    ProfilerEvent::ProfilerEvent(
        std::chrono::high_resolution_clock::time_point time,
        std::unique_ptr<EventMeta> meta
    ) : time(time), _meta(std::move(meta)) {

    }

    ProfilerEvent::ProfilerEvent(
        ProfilerEvent&& moved
    ) : time(std::move(moved.time)),
        _meta(std::move(moved._meta)) {

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

    const std::unique_ptr<un::EventMeta>& ProfilerEvent::getMeta() const {
        return _meta;
    }

    void EventHistory::record(std::unique_ptr<EventMeta>&& meta) {
        std::chrono::high_resolution_clock::time_point point = std::chrono::high_resolution_clock::now();
        {
            std::lock_guard<std::mutex> lock(_queueMutex);
            _events.emplace_back(point, std::move(meta));
        }
    }

    EventHistory::EventHistory() : _events() {

    }

    const std::vector<un::ProfilerEvent>& EventHistory::getEvents() const {
        return _events;
    }

}