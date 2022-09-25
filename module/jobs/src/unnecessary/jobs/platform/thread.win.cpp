#include <unnecessary/jobs/thread.h>
#include <windows.h>

namespace un {
    DWORD WINAPI UnnecessaryThreadProc(LPVOID lpParameter) {
        reinterpret_cast<un::Thread*>(lpParameter)->operator()();
        return 0;
    }

    void Thread::start() {
        size_t stackSize = 4096;
        auto hThread = _nativeHandle = CreateThread(
            nullptr,
            _params.getStackSize(),
            reinterpret_cast<LPTHREAD_START_ROUTINE>(&UnnecessaryThreadProc),
            this,
            CREATE_SUSPENDED,
            nullptr
        );

        const std::string& _name = _params.getName();
        if (!_name.empty()) {
            std::wstring wideStr(
                _name.begin(),
                _name.end()
            );
            SetThreadDescription(hThread, wideStr.c_str());
        }

        size_t core = _params.getCore();
        if (core != -1) {
            SetThreadAffinityMask(hThread, 1 << core);
        }

        ResumeThread(hThread);
    }
}