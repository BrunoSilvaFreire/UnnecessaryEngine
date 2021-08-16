#ifndef UNNECESSARYENGINE_EVENT_H
#define UNNECESSARYENGINE_EVENT_H

#include <vector>
#include <functional>

namespace un {
    template<typename... T>
    class Event {
    public:
        typedef std::function<void(T...)> EventListener;
    private:
        std::vector<EventListener> listeners;
    public:
        void clear() {
            listeners.clear();
        }

        void operator+=(EventListener listener) {
            listeners.push_back(listener);
        }

        void operator-=(EventListener listener) {
            listeners.erase(std::remove(listeners.begin(), listeners.end(), listener));
        }

        void operator()(T... args) {
            for (EventListener& listener : listeners) {
                listener(args...);
            }
        }
    };
}
#endif