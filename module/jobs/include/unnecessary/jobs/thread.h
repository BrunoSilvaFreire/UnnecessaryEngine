//
// Created by bruno on 20/01/2022.
//

#ifndef UNNECESSARYENGINE_THREAD_H
#define UNNECESSARYENGINE_THREAD_H

#include <thread>
#include <functional>
#include <mutex>
#include <unnecessary/def.h>

#if UN_PLATFORM_WINDOWS

#include <Windows.h>

#endif

namespace un {
    struct ThreadParams {
    private:
        std::string _name;
        std::size_t _core;
        std::size_t _stackSize;
    public:
        static const std::size_t kAnyCore = std::numeric_limits<std::size_t>::max();
        static const std::size_t kDefaultStackSize = 4096;

        ThreadParams(std::string name, std::size_t core = kAnyCore);

        const std::string& getName() const;

        size_t getCore() const;

        void setStackSize(size_t stackSize);

        size_t getStackSize() const;
    };

    struct ThreadPlatformBridge;

    class Thread {
    private:
        std::function<void()> _block;
        std::mutex _dataMutex;
        bool _alive;
        un::ThreadParams _params;
        un::ptr<ThreadPlatformBridge> _bridge;

        void operator()();

        void setThreadName(const std::string& name);

        bool setThreadCore(u32 core);

    public:
        explicit Thread(const std::function<void()>& block);

        explicit Thread(un::ThreadParams name, const std::function<void()>& block);

        Thread() = delete;

        Thread(const Thread&) = delete;

        void start();

        void join();

#if UN_PLATFORM_WINDOWS

        friend DWORD WINAPI UnnecessaryThreadProc(_In_ LPVOID lpParameter);
#elif UN_PLATFORM_UNIX

        friend un::void_ptr unnecessary_unix_proc(un::void_ptr ptr);

#endif
    };
}
#endif
