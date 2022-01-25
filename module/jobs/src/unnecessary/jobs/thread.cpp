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
        if (block == nullptr){
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

#ifdef WIN32

    bool Thread::setAffinityMask(std::size_t mask) {
        HANDLE hThread = reinterpret_cast<HANDLE>(_inner.native_handle());
        auto returnStatus = SetThreadAffinityMask(hThread, mask);
        return returnStatus != 0;
    }

    void Thread::setName(const std::string& name) {
        HANDLE hThread = reinterpret_cast<HANDLE>(_inner.native_handle());
        SetThreadDescription(
            hThread,
            std::wstring(
                name.begin(),
                name.end()
            ).c_str()
        );
    }
#else

    bool Thread::setAffinityMask(std::size_t mask) {
        cpu_set_t cpu;
        CPU_ZERO(&cpu);
        CPU_SET(mask, &cpu);
        int status = pthread_setaffinity_np(
            _inner.native_handle(),
            sizeof(cpu_set_t),
            &cpu
        );
        return status == 0;
    }

    void Thread::setName(const std::string& name) {
        pthread_setname_np(
            _inner.native_handle(),
            name.c_str()
        );
    }

#endif
}