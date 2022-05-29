#ifndef UNNECESSARYENGINE_EVENT_H
#define UNNECESSARYENGINE_EVENT_H

#include <vector>
#include <functional>

namespace un {
    template<typename... T>
    class Event {
    public:
        typedef std::function<void(T...)> Listener;
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
}
#endif