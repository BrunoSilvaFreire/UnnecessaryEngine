#include <unnecessary/jobs/thread.h>
#include <unnecessary/logging.h>
#include <mutex>

namespace un {

    Thread::Thread(
        const std::function<void()>& block
    ) : _block(block),
        _inner(&Thread::operator(), this),
        _name(),
        _core(0) {
        if (block == nullptr) {
            throw std::runtime_error("Thread created with empty block.");
        }
    }

    Thread::Thread(
        const std::string& name,
        const std::function<void()>& block
    ) : Thread(block) {
        _alive = false;
        _nameDirty = false;
        _coreDirty = false;
        setName(name);
    }

    void Thread::operator()() {
        {
            _alive = true;
            if (_nameDirty) {
                setAliveThreadName(_name);
                _nameDirty = false;
            }
            if (_coreDirty) {
                setAliveThreadCore(_core);
                _coreDirty = false;
            }
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
        std::unique_lock<std::mutex> lock(_dataMutex);
        _core = core;
        if (_alive) {
            bool result = setAliveThreadCore(core);
            _coreDirty = false;
            return result;
        } else {
            _coreDirty = true;
            return false;
        }
    }

    void Thread::setName(const std::string& name) {
        std::unique_lock<std::mutex> lock(_dataMutex);
        _name = name;
        if (_alive) {
            setAliveThreadName(name);
            _nameDirty = false;
        } else {
            _nameDirty = true;
        }
    }
}