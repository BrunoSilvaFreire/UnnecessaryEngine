#ifndef UNNECESSARYENGINE_EVENT_HISTORY_H
#define UNNECESSARYENGINE_EVENT_HISTORY_H

#include <memory>
#include <vector>
#include <chrono>
#include <string>
#include <mutex>
#include <unnecessary/jobs/recorder/data/events.h>

namespace un {

    class ProfilerEvent {
    public:
        ProfilerEvent(
            std::chrono::high_resolution_clock::time_point time,
            std::unique_ptr<un::EventMeta> meta
        );

        ProfilerEvent(ProfilerEvent&& moved);

        ProfilerEvent(const ProfilerEvent& copy) = delete;

        std::chrono::high_resolution_clock::time_point getTime() const;

        const std::unique_ptr<un::EventMeta>& getMeta() const;

        bool operator<(const ProfilerEvent& rhs) const;

        bool operator>(const ProfilerEvent& rhs) const;

        bool operator<=(const ProfilerEvent& rhs) const;

        bool operator>=(const ProfilerEvent& rhs) const;

    protected:
        std::chrono::high_resolution_clock::time_point time;
        std::unique_ptr<un::EventMeta> _meta;

    };

    class EventHistory {
    private:
        std::mutex _queueMutex;
        std::vector<un::ProfilerEvent> _events;
    public:
        EventHistory();

        const std::vector<un::ProfilerEvent>& getEvents() const;

        void record(std::unique_ptr<EventMeta>&& meta);
    };

}
#endif
