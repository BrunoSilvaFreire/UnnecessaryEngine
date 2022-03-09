#ifndef UNNECESSARYENGINE_EVENT_HISTORY_H
#define UNNECESSARYENGINE_EVENT_HISTORY_H

#include <memory>
#include <vector>
#include <chrono>
#include <string>

namespace un {
    class EventMeta {
    public:
        EventMeta(std::string label);

        virtual ~EventMeta();

        const std::string& getLabel() const;

    private:
        std::string label;
    };

    class ProfilerEvent {
    public:
        ProfilerEvent(
            std::chrono::high_resolution_clock::time_point time,
            EventMeta* meta
        );

        ProfilerEvent(
            ProfilerEvent&& moved
        );

        ~ProfilerEvent();

        std::chrono::high_resolution_clock::time_point getTime() const;

        bool operator<(const ProfilerEvent& rhs) const;

        bool operator>(const ProfilerEvent& rhs) const;

        bool operator<=(const ProfilerEvent& rhs) const;

        bool operator>=(const ProfilerEvent& rhs) const;

    protected:
        std::chrono::high_resolution_clock::time_point time;
        EventMeta* _meta;

    };

    class EventHistory {
    private:
        std::vector<un::ProfilerEvent> _events;
    public:
        EventHistory();

        void record(EventMeta* meta);
    };

}
#endif
