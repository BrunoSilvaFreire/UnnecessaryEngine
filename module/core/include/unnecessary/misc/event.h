#ifndef UNNECESSARYENGINE_EVENT_H
#define UNNECESSARYENGINE_EVENT_H

#include <vector>
#include <functional>
#include <mutex>

namespace un {
    template<typename... T>
    class event {
    public:
        using listener = std::function<void(T...)>;

    private:
        std::vector<listener> _listeners;
        std::vector<listener> _singleFireListeners;

    public:
        void clear() {
            _listeners.clear();
        }

        void add_single_fire_listener(listener listener) {
            _singleFireListeners.push_back(listener);
        }

        void operator+=(listener listener) {
            _listeners.push_back(listener);
        }

        void operator-=(listener listener) {
            _listeners.erase(std::remove(_listeners.begin(), _listeners.end(), listener));
        }

        void operator()(T... args) {
            for (listener& listener : _listeners) {
                listener(args...);
            }
            for (listener& listener : _singleFireListeners) {
                listener(args...);
            }
            _singleFireListeners.clear();
        }
    };

    using event_void = event<>;

    template<typename... T>
    class thread_safe_event {
    public:
        using listener = typename event<T...>::listener;

    private:
        event<T...> _inner;
        std::mutex _mutex;

    public:
        void clear() {
            std::lock_guard<std::mutex> lock(_mutex);
            _inner.clear();
        }

        void add_single_fire_listener(listener listener) {
            std::lock_guard<std::mutex> lock(_mutex);
            _inner.add_single_fire_listener(listener);
        }

        void operator+=(listener listener) {
            std::lock_guard<std::mutex> lock(_mutex);
            _inner += listener;
        }

        void operator-=(listener listener) {
            std::lock_guard<std::mutex> lock(_mutex);
            _inner -= listener;
        }

        void operator()(T... args) {
            std::lock_guard<std::mutex> lock(_mutex);
            _inner(args...);
        }
    };
}
#endif
