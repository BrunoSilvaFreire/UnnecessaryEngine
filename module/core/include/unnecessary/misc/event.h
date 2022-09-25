#ifndef UNNECESSARYENGINE_EVENT_H
#define UNNECESSARYENGINE_EVENT_H

#include <vector>
#include <functional>
#include <mutex>

namespace un {
    template<typename... T>
    class Event {
    public:
        using Listener = std::function<void(T...)>;
    private:
        std::vector<Listener> listeners;
        std::vector<Listener> singleFireListeners;
    public:
        void clear() {
            listeners.clear();
        }

        void addSingleFireListener(Listener listener) {
            singleFireListeners.push_back(listener);
        }

        void operator+=(Listener listener) {
            listeners.push_back(listener);
        }

        void operator-=(Listener listener) {
            listeners.erase(std::remove(listeners.begin(), listeners.end(), listener));
        }

        void operator()(T... args) {
            for (Listener& listener : listeners) {
                listener(args...);
            }
            for (Listener& listener : singleFireListeners) {
                listener(args...);
            }
            singleFireListeners.clear();
        }
    };

    typedef un::Event<> EventVoid;

    template<typename... T>
    class ThreadSafeEvent {
    public:
        using Listener = typename Event<T...>::Listener;
    private:
        Event<T...> _inner;
        std::mutex _mutex;
    public:
        void clear() {
            std::lock_guard<std::mutex> lock(_mutex);
            _inner.clear();
        }

        void addSingleFireListener(Listener listener) {
            std::lock_guard<std::mutex> lock(_mutex);
            _inner.addSingleFireListener(listener);
        }

        void operator+=(Listener listener) {
            std::lock_guard<std::mutex> lock(_mutex);
            _inner += listener;
        }

        void operator-=(Listener listener) {
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