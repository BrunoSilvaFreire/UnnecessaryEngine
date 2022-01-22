#include <unnecessary/jobs/thread.h>

#ifdef WIN32

#include <Windows.h>

#endif
namespace un {
#ifdef WIN32

    bool Thread::setAffinityMask(std::size_t mask) {
        HANDLE hThread = reinterpret_cast<HANDLE>(_inner.native_handle());
        auto returnStatus = SetThreadAffinityMask(hThread, mask);
        return returnStatus == 0;
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

    Thread::Thread(
        const std::function<void()>& block
    ) : _block(block), _inner(
        &Thread::operator(), this
    ) {}

    Thread::Thread(const std::string& name, const std::function<void()>& block) : Thread(block) {
        setName(name);
    }

    void Thread::operator()() {
        _block();
    }

#else
#endif
}