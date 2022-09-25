#ifndef UNNECESSARYENGINE_FENCE_H
#define UNNECESSARYENGINE_FENCE_H

#include <mutex>
#include <set>
#include <sstream>

namespace un {
    template<typename TValue = std::size_t>
    class Fence {
    public:
        using ValueType = TValue;

    private:
        std::mutex _mutex, _completed;
        std::condition_variable _variable;
        std::set<TValue> _remaining;
#if DEBUG
        std::set<TValue> _all;
#endif
    public:
        explicit Fence(
            const std::set<TValue>& remaining
        ) : _remaining(remaining)
#if DEBUG
            , _all(remaining)
#endif
        { }

        void notify(TValue id) {
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
            } else {
                _mutex.unlock();
            }
        }
    };

    template<typename TValue = std::size_t>
    class FenceNotifier {
    private:
        un::Fence<TValue>* _fence;
        TValue _value;
    public:
        FenceNotifier(Fence<TValue>* fence, TValue value) : _fence(fence), _value(value) { }

        FenceNotifier(const FenceNotifier<TValue>&) = default;

        FenceNotifier(FenceNotifier<TValue>&&) = delete;

        void notify() {
            _fence->notify(_value);
        }
    };

    namespace fences {

        template<typename TFence, typename TIterator, typename TLambda>
        static void waitFor(
            TIterator begin,
            TIterator end,
            TLambda dispatcher
        ) {
            using TValue = typename TFence::ValueType;
            std::set<TValue> pending;
            std::size_t i = 0;
            std::for_each(
                begin, end,
                [&](auto& val) {
                    pending.emplace(i++);
                }
            );
            un::Fence<TValue> fence(pending);
            std::size_t j = 0;
            std::for_each(
                begin, end,
                [&](auto& val) {
                    dispatcher(val, un::FenceNotifier<TValue>(&fence, j++));
                }
            );
            fence.wait();
        }

        template<typename TIterator, typename TLambda>
        static void waitFor(
            TIterator begin,
            TIterator end,
            TLambda dispatcher
        ) {
            waitFor<un::Fence<>>(begin, end, dispatcher);
        }
    }
}
#endif
