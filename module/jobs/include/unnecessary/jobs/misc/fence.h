#ifndef UNNECESSARYENGINE_FENCE_H
#define UNNECESSARYENGINE_FENCE_H

#include <mutex>
#include <set>
#include <sstream>

namespace un {
    template<typename TValue = std::size_t>
    class Fence {
    private:
        std::mutex mutex, completed;
        std::condition_variable variable;
        std::set<TValue> remaining;
    public:
        explicit Fence(const std::set<TValue> &remaining) : remaining(remaining) {}

        void notify(TValue id) {
            std::lock_guard<std::mutex> lock(mutex);
#if DEBUG
            if (!remaining.contains(id)) {
                std::stringstream ss;
                ss << "Id " << id << " either has been notified multiple times or is invalid.";
                throw std::runtime_error(ss.str());
            }
#endif
            remaining.erase(id);
            if (remaining.empty()) {
                variable.notify_all();
            }
        }

        void wait() {
            mutex.lock();
            if (!remaining.empty()) {
                std::unique_lock<std::mutex> unique(completed);
                mutex.unlock();
                variable.wait(unique);
            } else {
                mutex.unlock();
            }
        }
    };
}
#endif
