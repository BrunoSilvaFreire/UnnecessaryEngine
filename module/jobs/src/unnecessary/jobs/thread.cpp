#include <unnecessary/jobs/thread.h>
#include <unnecessary/logging.h>
#include <mutex>

namespace un {

    Thread::Thread(
        const std::function<void()>& block
    ) : _block(block),
        _alive(false),
        _inner() {
        if (block == nullptr) {
            throw std::runtime_error("Thread created with empty block.");
        }
    }

    Thread::Thread(
        const std::string& name,
        const std::function<void()>& block
    ) : Thread(block) {

        setName(name);
    }

    void Thread::operator()() {
        {
            std::unique_lock<std::mutex> lock(_dataMutex);

            std::thread::id id = _inner.get_id();
            _alive = true;
        }
        _block();
        {
            std::unique_lock<std::mutex> lock(_dataMutex);
            _alive = false;
        }
    }

    void Thread::join() {
        _inner.join();
    }

    bool Thread::isAlive() {
        std::unique_lock<std::mutex> lock(_dataMutex);
        return _alive;
    }

    bool Thread::setCore(u32 core) {
        Thread::core = core;
        if (_alive) {
            return setThreadCore(core);
        }
        return true;
    }

    void Thread::setName(const std::string& name) {
        Thread::name = name;
        if (_alive) {
            setThreadName(name);
        }
    }

    void Thread::start() {
        _inner = std::thread(&Thread::operator(), this);
    }
}