#include <unnecessary/jobs/thread.h>

#ifdef WIN32
#include <Windows.h>
#else
#include <pthread.h>
#endif
namespace un {

    Thread::Thread(
        const std::function<void()>& block
    ) : _block(block),
        _inner(&Thread::operator(), this) {
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
        _block();
    }

    void Thread::join() {
        _inner.join();
    }

#ifdef WIN32


#else

#endif
}