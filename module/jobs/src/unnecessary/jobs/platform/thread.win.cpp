#include <unnecessary/jobs/thread.h>
#include <windows.h>

namespace un {
    struct thread_platform_bridge {
    public:
        HANDLE threadHandle;
    };

    DWORD WINAPI
    unnecessary_thread_proc(LPVOID
    lpParameter) {
    reinterpret_cast<thread*>(lpParameter)->
    operator()();
    return 0;
}

void thread::join() {
    WaitForSingleObject(_bridge->threadHandle, INFINITE);
}

void thread::start() {
    size_t stackSize = 4096;
    _bridge = new thread_platform_bridge();
    auto hThread = _bridge->threadHandle = CreateThread(
        nullptr,
        _params.get_stack_size(),
        &unnecessary_thread_proc,
        this,
        CREATE_SUSPENDED,
        nullptr
    );

    const std::string& name = _params.get_name();
    if (!name.empty()) {
        std::wstring wideStr(
            name.begin(),
            name.end()
        );
        SetThreadDescription(hThread, wideStr.c_str());
    }

    size_t core = _params.get_core();
    if (core != -1) {
        SetThreadAffinityMask(hThread, 1 << core);
    }

    ResumeThread(hThread);
}
}
