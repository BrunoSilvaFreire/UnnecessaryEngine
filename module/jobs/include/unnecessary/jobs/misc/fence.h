#ifndef UNNECESSARYENGINE_FENCE_H
#define UNNECESSARYENGINE_FENCE_H

#include <mutex>
#include <set>
#include <sstream>

namespace un {
    template<typename t_value = std::size_t>
    class fence {
    public:
        using value_type = t_value;

    private:
        std::mutex _mutex, _completed;
        std::condition_variable _variable;
        std::set<t_value> _remaining;
#if DEBUG
        std::set<t_value> _all;
#endif

    public:
        explicit fence(
            const std::set<t_value>& remaining
        ) : _remaining(remaining)
#if DEBUG
            , _all(remaining)
#endif
        {
        }

        void notify(t_value id) {
            std::lock_guard<std::mutex> lock(_mutex);
#if DEBUG
            if (!_remaining.contains(id)) {
                std::stringstream ss;
                ss << "Id " << id << " either has been notified multiple times or is invalid.";
                throw std::runtime_error(ss.str());
            }
#endif
            _remaining.erase(id);
            if (_remaining.empty()) {
                _variable.notify_all();
            }
        }

        void wait() {
            _mutex.lock();
            if (!_remaining.empty()) {
                std::unique_lock<std::mutex> unique(_completed);
                _mutex.unlock();
                _variable.wait(unique);
            }
            else {
                _mutex.unlock();
            }
        }
    };

    template<typename t_value = std::size_t>
    class fence_notifier {
    private:
        fence<t_value>* _fence;
        t_value _value;

    public:
        fence_notifier(fence<t_value>* fence, t_value value) : _fence(fence), _value(value) {
        }

        fence_notifier(const fence_notifier<t_value>&) = default;

        fence_notifier(fence_notifier<t_value>&&) = delete;

        void notify() {
            _fence->notify(_value);
        }
    };

    namespace fences {
        template<typename t_fence, typename t_iterator, typename t_lambda>
        static void wait_for(
            t_iterator begin,
            t_iterator end,
            t_lambda dispatcher
        ) {
            using t_value = typename t_fence::value_type;
            std::set<t_value> pending;
            std::size_t i = 0;
            std::for_each(
                begin, end,
                [&](auto& val) {
                    pending.emplace(i++);
                }
            );
            fence<t_value> fence(pending);
            std::size_t j = 0;
            std::for_each(
                begin, end,
                [&](auto& val) {
                    dispatcher(val, un::fence_notifier<t_value>(&fence, j++));
                }
            );
            fence.wait();
        }

        template<typename t_iterator, typename t_lambda>
        static void wait_for(
            t_iterator begin,
            t_iterator end,
            t_lambda dispatcher
        ) {
            wait_for<fence<>>(begin, end, dispatcher);
        }
    }
}
#endif
