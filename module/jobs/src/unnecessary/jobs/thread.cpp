#include <unnecessary/jobs/thread.h>
#include <unnecessary/logging.h>
#include <mutex>
#include <utility>

namespace un {
    thread::thread(
        const std::function<void()>& block
    ) : thread(thread_params("UnnecessaryThread"), block) {
    }

    thread::thread(
        thread_params parameters,
        const std::function<void()>& block
    ) : _block(block),
        _alive(false),
        _params(std::move(parameters)),
        _bridge(nullptr) {
        if (block == nullptr) {
            throw std::runtime_error("Thread created with empty block.");
        }
    }

    void thread::operator()() {
        std::unique_lock<std::mutex> lock(_dataMutex);
        _alive = true;
        _block();
        _alive = false;
    }

    thread_params::thread_params(
        std::string name,
        size_t core
    ) : _name(std::move(name)),
        _core(core),
        _stackSize(k_default_stack_size) {
    }

    const std::string& thread_params::get_name() const {
        return _name;
    }

    size_t thread_params::get_core() const {
        return _core;
    }

    size_t thread_params::get_stack_size() const {
        return _stackSize;
    }

    void thread_params::set_stack_size(size_t stackSize) {
        _stackSize = stackSize;
    }
}
