#include <unnecessary/jobs/thread.h>
#include <windows.h>

namespace un {

    bool Thread::setAliveThreadCore(u32 core) {
        HANDLE hThread = reinterpret_cast<HANDLE>(_inner.native_handle());
        auto returnStatus = SetThreadAffinityMask(hThread, 1 << core);
        return returnStatus != 0;
    }

    void Thread::setAliveThreadName(const std::string& name) {
        HANDLE hThread = reinterpret_cast<HANDLE>(_inner.native_handle());
        SetThreadDescription(
            hThread,
            std::wstring(
                name.begin(),
                name.end()
            ).c_str()
        );
    }

}