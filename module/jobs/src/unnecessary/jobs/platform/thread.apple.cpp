#include <unnecessary/jobs/thread.h>

//Unsupported
namespace un {
    bool Thread::setAliveThreadCore(u32 core) {
        return false;
    }

    void Thread::setAliveThreadName(const std::string& name) {
    }

    void Thread::start() {
        _inner = std::thread(&Thread::operator(), this);
    }
}